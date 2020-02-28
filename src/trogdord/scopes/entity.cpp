#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/io/output/driver.h"

#include "../include/scopes/entity.h"
#include "../include/exception/entitynotfound.h"


// Scope name that should be used in requests
const char *EntityController::SCOPE = "entity";

// Actions served by the "entity" scope
const char *EntityController::LIST_ACTION = "list";
const char *EntityController::OUTPUT_ACTION = "output";
const char *EntityController::INPUT_ACTION = "input";

// Error messages
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

	// TODO: I need a good hierarchical way to get additional type-specific
	// properties for the entity.

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

	JSONObject response;

	int gameId;
	std::string entityName;

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

	std::unique_ptr<trogdor::Game> &game = GameContainer::get()->getGame(gameId);

	if (!game) {

		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);

		return response;
	}

	try {

		trogdor::entity::Entity *ePtr = getEntityPtr(game, entityName);

		response.put("status", 200);
		response.add_child("entity", entityToJSONObject(ePtr));
	}

	catch (EntityNotFound &e) {
		response.put("status", 404);
		response.put("message", ENTITY_NOT_FOUND);	
	}

	return response;
}

/*****************************************************************************/

JSONObject EntityController::getEntityList(JSONObject request) {

	int gameId;

	JSONObject response;

	try {
		gameId = Request::parseGameId(request, "args.game_id");
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<trogdor::Game> &game = GameContainer::get()->getGame(gameId);

	if (game) {

		JSONObject entities;

		for (const auto &entity: getEntityPtrList(game)) {
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

	std::unique_ptr<output::Driver> &outBuffer = output::Driver::get(
		Config::get()->value<std::string>(Config::CONFIG_KEY_OUTPUT_DRIVER)
	);

	response.put("status", 200);
	response.put("message", "TODO");

	return response;
}

/*****************************************************************************/

JSONObject EntityController::appendOutput(JSONObject request) {

	JSONObject response;

	response.put("status", 200);
	response.put("message", "TODO");

	return response;
}

/*****************************************************************************/

JSONObject EntityController::postInput(JSONObject request) {

	JSONObject response;

	response.put("status", 200);
	response.put("message", "TODO");

	return response;
}
