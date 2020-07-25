#include <trogdor/entities/entity.h>

#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/io/output/driver.h"

#include "../include/scopes/entity.h"
#include "../include/exception/unsupportedoperation.h"
#include "../include/exception/entity/entitynotfound.h"


// Scope name that should be used in requests
const char *EntityController::SCOPE = "entity";

// Actions served by the "entity" scope
const char *EntityController::LIST_ACTION = "list";
const char *EntityController::OUTPUT_ACTION = "output";

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

	trogdor::entity::Entity *ePtr = game->getEntity(entityName).get();

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
	trogdor::entity::Entity *&ePtr,
	std::string missingNameMsg,
	std::string invalidNameMsg,
	std::string notFoundMsg
) {

	try {
		gameId = Request::parseGameId(request, "args.game_id");
		entityName = Request::parseArgument<std::string>(
			request,
			"args.name",
			missingNameMsg,
			invalidNameMsg
		);
	}

	catch (const JSONObject &error) {
		return error;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (!game) {

		JSONObject response;

		response.put("status", Response::STATUS_NOT_FOUND);
		response.put("message", GAME_NOT_FOUND);

		return response;
	}

	try {
		ePtr = getEntityPtr(game->get(), entityName);
		return std::nullopt;
	}

	catch (const EntityNotFound &e) {

		JSONObject response;

		response.put("status", Response::STATUS_NOT_FOUND);
		response.put("message", notFoundMsg);

		return response;
	}
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> EntityController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> entities;

	for (const auto &entity: game->getEntities()) {
		entities.push_back(entity.second.get());
	}

	return entities;
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

		response.put("status", Response::STATUS_SUCCESS);
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

	catch (const JSONObject &error) {
		return error;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {

		JSONObject entities;

		for (const auto &entity: getEntityPtrList(game->get())) {
			entities.push_back(std::make_pair("",
				entityToJSONObject(entity)
			));
		}

		// See comment in GameController::getGameList describing use of
		// addedGames for an explanation of what I'm doing here.
		if (!entities.size()) {
			entities.push_back(std::make_pair("", JSONObject()));
		}

		response.put("status", Response::STATUS_SUCCESS);
		response.add_child("entities", entities);
	}

	else {
		response.put("status", Response::STATUS_NOT_FOUND);
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

	catch (const JSONObject &error) {
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

	try {

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

		response.put("status", Response::STATUS_SUCCESS);
		response.add_child("messages", messages);
	}

	catch (const UnsupportedOperation &e) {
		response.put("status", Response::STATUS_UNSUPPORTED);
		response.put("message", e.what());
	}

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

		response.put("status", Response::STATUS_INVALID);
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

	response.put("status", Response::STATUS_SUCCESS);
	return response;
}
