#include <utility>

#include <trogdor/utility.h>
#include <trogdor/filesystem.h>

#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/filesystem.h"

#include "../include/scopes/game.h"

// Scope name that should be used in requests
const char *GameController::SCOPE = "game";

// Actions served by the "game" scope
const char *GameController::LIST_ACTION = "list";
const char *GameController::DEFINITIONS_ACTION = "definitions";

// Error messages
const char *GameController::MISSING_REQUIRED_NAME = "missing required name";
const char *GameController::MISSING_REQUIRED_DEFINITION = "missing required definition path";
const char *GameController::DEFINITION_NOT_RELATIVE = "definition path must be relative";

// Singleton instance of GameController
std::unique_ptr<GameController> GameController::instance = nullptr;

/*****************************************************************************/

GameController::GameController() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getGame(request);
	});

	registerAction(Request::GET, LIST_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getGameList(request);
	});

	registerAction(Request::GET, DEFINITIONS_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getDefinitionList(request);
	});

	registerAction(Request::POST, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->createGame(request);
	});

	registerAction(Request::DELETE, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->destroyGame(request);
	});
}

/*****************************************************************************/

std::unique_ptr<GameController> &GameController::get() {

	if (!instance) {
		instance = std::unique_ptr<GameController>(new GameController());
	}

	return instance;
}

/*****************************************************************************/

JSONObject GameController::getGame(JSONObject request) {

	int gameId;
	JSONObject response;

	try {
		gameId = parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<trogdor::Game> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.put("status", 200);
		response.put("id", gameId);
		response.put("name", game->getMeta("name"));
		response.put("current_time", game->getTime());
		response.put("running", game->inProgress());
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::getGameList(JSONObject request) {

	JSONObject response;
	JSONObject gameList;

	auto &gamePtrs = GameContainer::get()->getGames();

	for (size_t i = 0; i < gamePtrs.size(); i++) {

		if (gamePtrs[i]) {

			JSONObject game;

			game.put("id", i);
			game.put("name", gamePtrs[i]->getMeta("name"));
			gameList.push_back(std::make_pair("", game));
		}
	}

	// This kludge, if there are no games in GameContainer to list, results in
	// write_json() outputting [""], which the serialize() function in json.cpp
	// will later convert to [].
	if (!gameList.size()) {
		gameList.push_back(std::make_pair("", JSONObject()));
	}

	response.put("status", 200);
	response.add_child("games", gameList);

	return response;
}

/*****************************************************************************/

JSONObject GameController::getDefinitionList(JSONObject request) {

	JSONObject response;
	JSONObject definitions;

	std::string definitionsPath = Filesystem::getFullDefinitionsPath();

	try {

		for (auto &dirEntry: STD_FILESYSTEM::recursive_directory_iterator(definitionsPath)) {

			std::string filename = dirEntry.path().string();

			if (0 == filename.compare(filename.length() - 4, filename.length(), ".xml")) {

				JSONObject definition;

				definition.put_value(filename.replace(0, definitionsPath.length(), ""));
				definitions.push_back(std::make_pair("", definition));
			}
		}

		// See comment in GameController::getGameList describing use of
		// addedGames for an explanation of what I'm doing here.
		if (!definitions.size()) {
			definitions.push_back(std::make_pair("", JSONObject()));
		}

		response.put("status", 200);
		response.add_child("definitions", definitions);
	}

	catch (STD_FILESYSTEM::filesystem_error &e) {
		response.put("status", 500);
		response.put("message", e.what());
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::createGame(JSONObject request) {

	JSONObject response;

	boost::optional name = request.get_optional<std::string>("args.name");
	boost::optional definition = request.get_optional<std::string>("args.definition");

	if (!name) {
		response.put("status", 400);
		response.put("message", MISSING_REQUIRED_NAME);
	}

	else if (!definition) {
		response.put("status", 400);
		response.put("message", MISSING_REQUIRED_DEFINITION);
	}

	else if (STD_FILESYSTEM::path(*definition).is_absolute()) {
		response.put("status", 400);
		response.put("message", DEFINITION_NOT_RELATIVE);
	}

	else {

		*name = trim(*name);
		*definition = trim(*definition);

		try {
			response.put("status", 200);
			response.put("id", GameContainer::get()->createGame(*name, *definition));
		}

		catch (trogdor::Exception &e) {
			response.put("status", 500);
			response.put("message", e.what());
		}

		catch (ServerException &e) {
			response.put("status", 500);
			response.put("message", e.what());
		}
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::destroyGame(JSONObject request) {

	int gameId;
	JSONObject response;

	try {
		gameId = parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	if (GameContainer::get()->getGame(gameId)) {
		GameContainer::get()->destroyGame(gameId);
		response.put("status", 200);
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}
