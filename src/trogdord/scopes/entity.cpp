#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/io/input/driver.h"
#include "../include/io/output/driver.h"

#include "../include/scopes/entity.h"
#include "../include/exception/entity/entitynotfound.h"


// Scope name that should be used in requests
const char *EntityController::SCOPE = "entity";

// Actions served by the "entity" scope
const char *EntityController::LIST_ACTION = "list";
const char *EntityController::OUTPUT_ACTION = "output";
const char *EntityController::INPUT_ACTION = "input";

// Error messages
const char *EntityController::MISSING_OUTPUT_MESSAGE = "missing required message";
const char *EntityController::MISSING_COMMAND = "missing required command";
const char *EntityController::MISSING_CHANNEL = "missing required channel";
const char *EntityController::INVALID_CHANNEL = "invalid channel";
const char *EntityController::MISSING_ENTITY_NAME = "missing required entity name";
const char *EntityController::INVALID_ENTITY_NAME = "invalid entity name";
const char *EntityController::ENTITY_NOT_FOUND = "entity not found";

// Singleton instance of EntityController
std::unique_ptr<EntityController> EntityController::instance = nullptr;

/*****************************************************************************/

JSONObject EntityController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject entity;

	entity.put("name", ePtr->getName());
	entity.put("type", ePtr->getTypeName());

	return entity;
}

/*****************************************************************************/

trogdor::entity::Entity *EntityController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getEntity(entityName);

	if (!ePtr) {
		throw EntityNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::optional<JSONObject> EntityController::getEntityHelper(
	JSONObject request,
	size_t &gameId,
	std::string &entityName,
	trogdor::entity::Entity *&ePtr
) {

	try {
		gameId = Request::parseGameId(request, "args.game_id");
		entityName = Request::parseArgument<std::string>(
			request,
			"args.name",
			MISSING_ENTITY_NAME,
			INVALID_ENTITY_NAME
		);
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (!game) {

		JSONObject response;

		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);

		return response;
	}

	try {
		ePtr = getEntityPtr(game->get(), entityName);
		return std::nullopt;
	}

	catch (EntityNotFound &e) {

		JSONObject response;

		response.put("status", 404);
		response.put("message", ENTITY_NOT_FOUND);

		return response;
	}
}

/*****************************************************************************/

const trogdor::entity::EntityMap EntityController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	return game->getEntities();
}

/*****************************************************************************/

EntityController::EntityController() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getEntity(request);
	});

	registerAction(Request::GET, LIST_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getEntityList(request);
	});

	registerAction(Request::GET, OUTPUT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getOutput(request);
	});

	registerAction(Request::POST, OUTPUT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->appendOutput(request);
	});

	registerAction(Request::POST, INPUT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->postInput(request);
	});
}

/*****************************************************************************/

std::unique_ptr<EntityController> &EntityController::get() {

	if (!instance) {
		instance = std::unique_ptr<EntityController>(new EntityController());
	}

	return instance;
}

/*****************************************************************************/

JSONObject EntityController::getEntity(JSONObject request) {

	size_t gameId;
	std::string entityName;
	trogdor::entity::Entity *ePtr;

	std::optional<JSONObject> error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr
	);

	if (error.has_value()) {
		return *error;
	}

	else {

		JSONObject response;

		response.put("status", 200);
		response.add_child("entity", entityToJSONObject(ePtr));

		return response;
	}
}

/*****************************************************************************/

JSONObject EntityController::getEntityList(JSONObject request) {

	size_t gameId;

	JSONObject response;

	try {
		gameId = Request::parseGameId(request, "args.game_id");
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {

		JSONObject entities;

		for (const auto &entity: getEntityPtrList(game->get())) {
			entities.push_back(std::make_pair("",
				entityToJSONObject(entity.second.get())
			));
		}

		// See comment in GameController::getGameList describing use of
		// addedGames for an explanation of what I'm doing here.
		if (!entities.size()) {
			entities.push_back(std::make_pair("", JSONObject()));
		}

		response.put("status", 200);
		response.add_child("entities", entities);
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject EntityController::getOutput(JSONObject request) {

	JSONObject response;

	size_t gameId;
	trogdor::entity::Entity *ePtr;

	std::string entityName;
	std::string channel;

	std::unique_ptr<output::Driver> &outBuffer = output::Driver::get(
		Config::get()->value<std::string>(Config::CONFIG_KEY_OUTPUT_DRIVER)
	);

	try {
		channel = Request::parseArgument<std::string>(
			request,
			"args.channel",
			MISSING_CHANNEL,
			INVALID_CHANNEL
		);
	}

	catch (JSONObject error) {
		return error;
	}

	std::optional<JSONObject> error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr
	);

	if (error.has_value()) {
		return *error;
	}

	JSONObject messages;

	for (
		std::optional<output::Message> m = outBuffer->pop(gameId, entityName, channel);
		m.has_value();
		m = outBuffer->pop(gameId, entityName, channel)
	) {
		messages.push_back(std::make_pair("", (*m).toJSONObject()));
	};

	// See comment in GameController::getGameList describing use of addedGames
	// for an explanation of what I'm doing here.
	if (!messages.size()) {
		messages.push_back(std::make_pair("", JSONObject()));
	}

	response.put("status", 200);
	response.add_child("messages", messages);

	return response;
}

/*****************************************************************************/

JSONObject EntityController::appendOutput(JSONObject request) {

	JSONObject response;

	size_t gameId;
	trogdor::entity::Entity *ePtr;

	std::string entityName;
	std::string channel = trogdor::entity::Entity::DEFAULT_OUTPUT_CHANNEL;

	boost::optional channelArg = request.get_optional<std::string>("args.channel");
	boost::optional messageArg = request.get_optional<std::string>("args.message");

	if (channelArg) {
		channel = *channelArg;
	}

	if (!messageArg) {

		response.put("status", 400);
		response.put("message", MISSING_OUTPUT_MESSAGE);

		return response;
	}

	std::optional<JSONObject> error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr
	);

	if (error.has_value()) {
		return *error;
	}

	ePtr->out(channel) << *messageArg << std::endl;

	response.put("status", 200);
	return response;
}

/*****************************************************************************/

JSONObject EntityController::postInput(JSONObject request) {

	JSONObject response;

	size_t gameId;
	trogdor::entity::Entity *ePtr;
	std::string entityName;

	std::optional<JSONObject> error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr
	);

	if (error.has_value()) {
		return *error;
	}

	boost::optional command = request.get_optional<std::string>("args.command");

	if (!command) {
		response.put("status", 400);
		response.put("message", MISSING_COMMAND);
	}

	else {

		std::unique_ptr<input::Driver> &inBuffer = input::Driver::get(
			Config::get()->value<std::string>(Config::CONFIG_KEY_INPUT_DRIVER)
		);

		inBuffer->set(gameId, entityName, *command);
		response.put("status", 200);
	}

	return response;
}
