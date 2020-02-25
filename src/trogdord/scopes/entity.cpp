#include "../include/request.h"
#include "../include/gamecontainer.h"

#include "../include/scopes/entity.h"


// Scope name that should be used in requests
const char *EntityController::SCOPE = "entity";

// Actions served by the "entity" scope
const char *EntityController::LIST_ACTION = "list";

// Error messages
const char *EntityController::MISSING_ENTITY_NAME = "missing required entity name";
const char *EntityController::INVALID_ENTITY_NAME = "invalid entity name";
const char *EntityController::ENTITY_NOT_FOUND = "entity not found";

// Singleton instance of EntityController
std::unique_ptr<EntityController> EntityController::instance = nullptr;

/*****************************************************************************/

JSONObject EntityController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject entity;

	// TODO: what other values should I include here?
	entity.put("name", ePtr->getName());
	entity.put("type", ePtr->getTypeName());

	return entity;
}

/*****************************************************************************/

EntityController::EntityController() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getEntity(request);
	});

	registerAction(Request::GET, LIST_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getEntityList(request);
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

	trogdor::entity::Entity *ePtr = game->getEntity(entityName);

	if (!ePtr) {

		response.put("status", 404);
		response.put("message", ENTITY_NOT_FOUND);

		return response;
	};

	response.put("status", 200);
	response.add_child("entity", entityToJSONObject(ePtr));

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

		for (const auto &entity: game->getEntities()) {
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
