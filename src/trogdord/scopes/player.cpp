#include "../include/request.h"
#include "../include/gamecontainer.h"

#include "../include/scopes/player.h"


// Scope name that should be used in requests
const char *PlayerController::SCOPE = "player";

// Actions served by the "player" scope
const char *PlayerController::LIST_ACTION = "list";

// Error messages
const char *PlayerController::MISSING_PLAYER_NAME = "missing required player name";
const char *PlayerController::INVALID_PLAYER_NAME = "invalid player name";
const char *PlayerController::PLAYER_NOT_FOUND = "player not found";

// Singleton instance of PlayerController
std::unique_ptr<PlayerController> PlayerController::instance = nullptr;

/*****************************************************************************/

JSONObject PlayerController::playerToJSONObject(trogdor::entity::Player *pPtr) {

	JSONObject player;

	// TODO: what other values should I include here?
	player.put("name", pPtr->getName());

	return player;
}

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

	JSONObject response;

	int gameId;
	std::string playerName;

	try {
		gameId = Request::parseGameId(request, "args.game_id");
		playerName = Request::parseArgument<std::string>(
			request,
			"args.name",
			MISSING_PLAYER_NAME,
			INVALID_PLAYER_NAME
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

	trogdor::entity::Player *pPtr = game->getPlayer(playerName);

	if (!pPtr) {

		response.put("status", 404);
		response.put("message", PLAYER_NOT_FOUND);

		return response;
	};

	response.put("status", 200);
	response.add_child("player", playerToJSONObject(pPtr));

	return response;
}

/*****************************************************************************/

JSONObject PlayerController::getPlayerList(JSONObject request) {

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

	JSONObject response;

	int gameId;
	std::string playerName;

	try {
		gameId = Request::parseGameId(request, "args.game_id");
		playerName = Request::parseArgument<std::string>(
			request,
			"args.name",
			MISSING_PLAYER_NAME,
			INVALID_PLAYER_NAME
		);
	}

	catch (JSONObject error) {
		return error;
	}

	try {

		response.put("status", 200);
		response.add_child("player", playerToJSONObject(
			GameContainer::get()->createPlayer(gameId, playerName))
		);

		return response;
	}

	catch (GameNotFound &e) {

		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);

		return response;
	}

	catch (trogdor::Exception &e) {

		response.put("status", 500);
		response.put("message", e.what());

		return response;
	}
}

/*****************************************************************************/

JSONObject PlayerController::destroyPlayer(JSONObject request) {

	JSONObject response;

	int gameId;
	std::string playerName;

	// If a removal message is sent as part of the request, it will be sent to
	// the player's notifications channel before they're removed from the game.
	std::string removalMessage = "";

	boost::optional messageArg = request.get_optional<std::string>("args.message");

	if (messageArg) {
		removalMessage = *messageArg;
	}

	try {
		gameId = Request::parseGameId(request, "args.game_id");
		playerName = Request::parseArgument<std::string>(
			request,
			"args.name",
			MISSING_PLAYER_NAME,
			INVALID_PLAYER_NAME
		);
	}

	catch (JSONObject error) {
		return error;
	}

	try {

		GameContainer::get()->removePlayer(gameId, playerName, removalMessage);
		response.put("status", 200);

		return response;
	}

	catch (GameNotFound &e) {

		response.put("status", 404);
		response.put("message", GAME_NOT_FOUND);

		return response;
	}

	catch (PlayerNotFound &e) {

		response.put("status", 404);
		response.put("message", PLAYER_NOT_FOUND);

		return response;
	}

	catch (trogdor::Exception &e) {

		response.put("status", 500);
		response.put("message", e.what());

		return response;
	}
}
