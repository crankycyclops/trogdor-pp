#include "../include/request.h"
#include "../include/gamecontainer.h"

#include "../include/scopes/player.h"


// Scope name that should be used in requests
const char *PlayerController::SCOPE = "player";

// Actions served by the "player" scope
const char *PlayerController::LIST_ACTION = "list";

// Singleton instance of PlayerController
std::unique_ptr<PlayerController> PlayerController::instance = nullptr;

/*****************************************************************************/

PlayerController::PlayerController() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getPlayer(request);
	});

	registerAction(Request::GET, LIST_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->getPlayerList(request);
	});

	registerAction(Request::POST, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->createPlayer(request);
	});

	registerAction(Request::DELETE, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->destroyPlayer(request);
	});
}

/*****************************************************************************/

std::unique_ptr<PlayerController> &PlayerController::get() {

	if (!instance) {
		instance = std::unique_ptr<PlayerController>(new PlayerController());
	}

	return instance;
}

/*****************************************************************************/

JSONObject PlayerController::getPlayer(JSONObject request) {

	int gameId;

	JSONObject response;

	try {
		gameId = parseGameId(request, "args.game_id");
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<trogdor::Game> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.put("status", 200);
		response.put("message", "TODO");
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject PlayerController::getPlayerList(JSONObject request) {

	int gameId;
	JSONObject response;

	try {
		gameId = parseGameId(request, "args.game_id");
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<trogdor::Game> &game = GameContainer::get()->getGame(gameId);

	if (game) {

		JSONObject players;

		for (const auto &player: game->getPlayers()) {

			JSONObject playerObj;

			playerObj.put_value(player.second->getName());
			players.push_back(std::make_pair("", playerObj));
		}

		// See comment in GameController::getGameList describing use of
		// addedGames for an explanation of what I'm doing here.
		if (!players.size()) {
			players.push_back(std::make_pair("", JSONObject()));
		}

		response.put("status", 200);
		response.add_child("players", players);
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject PlayerController::createPlayer(JSONObject request) {

	int gameId;
	JSONObject response;

	try {
		gameId = parseGameId(request, "args.game_id");
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<trogdor::Game> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.put("status", 200);
		response.put("message", "TODO");
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}

/*****************************************************************************/

JSONObject PlayerController::destroyPlayer(JSONObject request) {

	int gameId;
	JSONObject response;

	try {
		gameId = parseGameId(request, "args.game_id");
	}

	catch (JSONObject error) {
		return error;
	}

	std::unique_ptr<trogdor::Game> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.put("status", 200);
		response.put("message", "TODO");
	}

	else {
		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);
	}

	return response;
}
