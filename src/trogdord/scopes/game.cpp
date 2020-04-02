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
const char *GameController::META_ACTION = "meta";
const char *GameController::DEFINITIONS_ACTION = "definitions";
const char *GameController::START_ACTION = "start";
const char *GameController::STOP_ACTION = "stop";
const char *GameController::TIME_ACTION = "time";
const char *GameController::IS_RUNNING_ACTION = "is_running";

// Error messages
const char *GameController::MISSING_REQUIRED_NAME = "missing required name";
const char *GameController::MISSING_REQUIRED_DEFINITION = "missing required definition path";
const char *GameController::DEFINITION_NOT_RELATIVE = "definition path must be relative";
const char *GameController::MISSING_META = "missing required meta key, value pairs";
const char *GameController::INVALID_META = "meta values cannot be objects or arrays";
const char *GameController::INVALID_META_KEYS = "invalid meta keys";

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

	registerAction(Request::GET, META_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getMeta(request);
	});

	registerAction(Request::GET, TIME_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getTime(request);
	});

	registerAction(Request::GET, IS_RUNNING_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getIsRunning(request);
	});

	registerAction(Request::GET, DEFINITIONS_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getDefinitionList(request);
	});

	registerAction(Request::POST, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->createGame(request);
	});

	registerAction(Request::SET, META_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->setMeta(request);
	});

	registerAction(Request::SET, START_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->startGame(request);
	});

	registerAction(Request::SET, STOP_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->stopGame(request);
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

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.put("status", 200);
		response.put("id", gameId);
		response.put("name", game->getName());
		response.put("definition", game->getDefinition());
		response.put("current_time", game->get()->getTime());
		response.put("is_running", game->get()->inProgress() ? "\\true\\" : "\\false\\");
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

	std::vector<std::string> metaKeys;
	auto includeMeta = request.get_child_optional("args.include_meta");

	auto &gamePtrs = GameContainer::get()->getGames();

	if (includeMeta && (*includeMeta).size()) {

		for (const auto &key: *includeMeta) {

			if (key.second.empty()) {
				metaKeys.push_back(key.second.data());
			}

			else {

				response.put("status", 400);
				response.put("message", INVALID_META_KEYS);

				return response;
			}
		}
	}

	for (size_t i = 0; i < gamePtrs.size(); i++) {

		if (gamePtrs[i]) {

			JSONObject game;

			game.put("id", i);
			game.put("name", gamePtrs[i]->getName());

			// If an include_meta argument is included, it specifies Game
			// meta data values that should be included along with the game's
			// ID and name in the returned list.
			for (const auto &key: metaKeys) {
				game.put(key, gamePtrs[i]->get()->getMeta(key));
			}

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

		*name = trogdor::trim(*name);
		*definition = trogdor::trim(*definition);

		// If any custom meta data was included in the request, set it when we
		// create the game.
		std::unordered_map<std::string, std::string> meta;

		for (const auto &requestItem: request.get_child("args")) {

			if (requestItem.first.compare("name") && requestItem.first.compare("definition")) {

				if (requestItem.second.empty()) {
					meta[requestItem.first] = requestItem.second.data();
				}

				else {

					response.put("status", 400);
					response.put("message", INVALID_META);

					return response;
				}
			}
		}

		try {
			response.put("status", 200);
			response.put("id", GameContainer::get()->createGame(*definition, *name, meta));
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

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
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

/*****************************************************************************/

JSONObject GameController::startGame(JSONObject request) {

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	if (GameContainer::get()->getGame(gameId)) {
		GameContainer::get()->startGame(gameId);
		response.put("status", 200);
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::stopGame(JSONObject request) {

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	if (GameContainer::get()->getGame(gameId)) {
		GameContainer::get()->stopGame(gameId);
		response.put("status", 200);
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::getMeta(JSONObject request) {

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	if (!GameContainer::get()->getGame(gameId)) {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	else {

		JSONObject meta;

		auto metaKeys = request.get_child_optional("args.meta");

		// Client is only requesting a certain set of meta values
		if (metaKeys && (*metaKeys).size()) {

			for (const auto &key: *metaKeys) {

				if (key.second.empty()) {
					meta.put(
						key.second.data(),
						GameContainer::get()->getMeta(gameId, key.second.data())
					);
				}

				else {

					response.put("status", 400);
					response.put("message", INVALID_META_KEYS);

					return response;
				}
			}
		}

		// Client is requesting all set meta values
		else {

			for (auto &metaVal: GameContainer::get()->getMetaAll(gameId)) {
				meta.put(metaVal.first, metaVal.second);
			}
		}

		response.put("status", 200);
		response.put_child("meta", meta);
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::setMeta(JSONObject request) {

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	if (!GameContainer::get()->getGame(gameId)) {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	else {

		auto meta = request.get_child_optional("args.meta");

		if (meta && (*meta).size()) {

			for (const auto &key: *meta) {

				if (key.second.empty()) {
					GameContainer::get()->setMeta(gameId, key.first, key.second.data());
				}

				else {

					response.put("status", 400);
					response.put("message", INVALID_META_KEYS);

					return response;
				}
			}

			response.put("status", 200);
		}

		else {
			response.put("status", 400);
			response.put("message", MISSING_META);
		}
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::getTime(JSONObject request) {

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.put("status", 200);
		response.put("current_time", game->get()->getTime());
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject GameController::getIsRunning(JSONObject request) {

	size_t gameId;
	JSONObject response;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.put("status", 200);
		response.put("is_running", game->get()->inProgress() ? "\\true\\" : "\\false\\");
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}
