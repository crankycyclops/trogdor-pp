#include "../include/request.h"
#include "../include/scopes/game.h"

// Scope name that should be used in requests
const char *Game::SCOPE = "game";

// Error messages
const char *Game::MISSING_GAME_ID = "missing required game id";
const char *Game::INVALID_GAME_ID = "invalid game id";

// Singleton instance of Game
std::unique_ptr<Game> Game::instance = nullptr;

/*****************************************************************************/

Game::Game() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getGame(request);
	});

	registerAction(Request::POST, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->createGame(request);
	});
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
		gameId = request.get<int>("args.id");
	}

	catch (boost::property_tree::ptree_bad_path &e) {

		response.put("status", 400);
		response.put("message", MISSING_GAME_ID);

		return response;
	}

	catch (boost::property_tree::ptree_bad_data &e) {

		response.put("status", 400);
		response.put("message", INVALID_GAME_ID);

		return response;
	}

	if (gameId < 0) {

		response.put("status", 400);
		response.put("message", INVALID_GAME_ID);

		return response;
	}

	// TODO: Retrieve game and throw 404 if not found
	response.put("status", 200);
	response.put("message", "TODO: get game stub");

	return response;
}

/*****************************************************************************/

JSONObject Game::createGame(JSONObject request) {

	JSONObject response;

	// "id" should be the game's id
	response.put("status", 200);
	response.put("id", 0);
	response.put("message", "TODO: create game stub");

	return response;
}
