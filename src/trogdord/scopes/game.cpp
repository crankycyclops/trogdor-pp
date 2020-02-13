#include <utility>
#include <trogdor/utility.h>

#include "../include/filesystem.h"
#include "../include/request.h"
#include "../include/gamecontainer.h"

#include "../include/scopes/game.h"

// Scope name that should be used in requests
const char *Game::SCOPE = "game";

// Actions served by the "game" scope
const char *Game::LIST_ACTION = "list";

// Error messages
const char *Game::MISSING_GAME_ID = "missing required game id";
const char *Game::INVALID_GAME_ID = "invalid game id";
const char *Game::GAME_NOT_FOUND = "game not found";
const char *Game::MISSING_REQUIRED_NAME = "missing required name";
const char *Game::MISSING_REQUIRED_DEFINITION = "missing required definition path";
const char *Game::DEFINITION_NOT_RELATIVE = "definition path must be relative";

// Singleton instance of Game
std::unique_ptr<Game> Game::instance = nullptr;

/*****************************************************************************/

Game::Game() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getGame(request);
	});

	registerAction(Request::GET, LIST_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getGameList(request);
	});

	registerAction(Request::POST, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->createGame(request);
	});

	registerAction(Request::DELETE, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->destroyGame(request);
	});
}

/*****************************************************************************/

int Game::parseGameId(JSONObject request) {

	try {

		int gameId = request.get<int>("args.id");

		if (gameId < 0) {

			JSONObject response;

			response.put("status", 400);
			response.put("message", INVALID_GAME_ID);

			throw response;
		}

		return gameId;
	}

	// It's kind of yucky catching an exception here just to throw another
	// one, but as you can see, in methods like getGame, it cleans up my code
	// a lot. Perhaps I can return a std::variant instead? I'll think about
	// this some more and revisit it.
	catch (boost::property_tree::ptree_bad_path &e) {

		JSONObject response;

		response.put("status", 400);
		response.put("message", MISSING_GAME_ID);

		throw response;
	}

	catch (boost::property_tree::ptree_bad_data &e) {

		JSONObject response;

		response.put("status", 400);
		response.put("message", INVALID_GAME_ID);

		throw response;
	}
}

/*****************************************************************************/

std::unique_ptr<Game> &Game::get() {

	if (!instance) {
		instance = std::unique_ptr<Game>(new Game());
	}

	return instance;
}

/*****************************************************************************/

JSONObject Game::getGame(JSONObject request) {

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

JSONObject Game::getGameList(JSONObject request) {

	JSONObject response;

	// TODO: stub with sample data for now
	JSONObject gameList;
	JSONObject testGame;

	testGame.put("id", 0);
	gameList.push_back(std::make_pair("", testGame));

	// TODO: remove message from this query on success
	response.put("status", 200);
	response.put("message", "TODO: get game list stub");
	response.add_child("games", gameList);

	return response;
}

/*****************************************************************************/

JSONObject Game::createGame(JSONObject request) {

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

JSONObject Game::destroyGame(JSONObject request) {

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
