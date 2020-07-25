#include "../include/request.h"
#include "../include/gamecontainer.h"

#include "../include/scopes/player.h"
#include "../include/exception/entity/playernotfound.h"

#include <trogdor/exception/duplicateentity.h>


// Scope name that should be used in requests
const char *PlayerController::SCOPE = "player";

// Actions served by the "Player" scope
const char *PlayerController::INPUT_ACTION = "input";

// Error messages
const char *PlayerController::MISSING_PLAYER_NAME = "missing required player name";
const char *PlayerController::INVALID_PLAYER_NAME = "invalid player name";
const char *PlayerController::PLAYER_NOT_FOUND = "player not found";

// Singleton instance of PlayerController
std::unique_ptr<PlayerController> PlayerController::instance = nullptr;

/*****************************************************************************/

JSONObject PlayerController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject player = BeingController::entityToJSONObject(ePtr);

	// TODO: add player-specific properties
	return player;
}

/*****************************************************************************/

trogdor::entity::Entity *PlayerController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getPlayer(entityName).get();

	if (!ePtr) {
		throw PlayerNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> PlayerController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> players;

	for (const auto &player: game->getPlayers()) {
		players.push_back(player.second.get());
	}

	return players;
}

/*****************************************************************************/

PlayerController::PlayerController(): BeingController() {

	registerAction(Request::POST, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->createPlayer(request);
	});

	registerAction(Request::DELETE, DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->destroyPlayer(request);
	});

	registerAction(Request::POST, INPUT_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->postInput(request);
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

JSONObject PlayerController::createPlayer(JSONObject request) {

	JSONObject response;

	size_t gameId;
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

	catch (const JSONObject &error) {
		return error;
	}

	try {

		response.put("status", Response::STATUS_SUCCESS);
		response.add_child("player", entityToJSONObject(
			GameContainer::get()->createPlayer(gameId, playerName))
		);

		return response;
	}

	catch (const GameNotFound &e) {

		response.put("status", Response::STATUS_NOT_FOUND);
		response.put("message", GAME_NOT_FOUND);

		return response;
	}

	catch (const trogdor::entity::DuplicateEntity &e) {

		response.put("status", Response::STATUS_CONFLICT);
		response.put("message", e.what());

		return response;
	}

	catch (const trogdor::Exception &e) {

		response.put("status", Response::STATUS_INTERNAL_ERROR);
		response.put("message", e.what());

		return response;
	}
}

/*****************************************************************************/

JSONObject PlayerController::destroyPlayer(JSONObject request) {

	JSONObject response;

	size_t gameId;
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

	catch (const JSONObject &error) {
		return error;
	}

	try {

		GameContainer::get()->removePlayer(gameId, playerName, removalMessage);
		response.put("status", Response::STATUS_SUCCESS);

		return response;
	}

	catch (const GameNotFound &e) {

		response.put("status", Response::STATUS_NOT_FOUND);
		response.put("message", GAME_NOT_FOUND);

		return response;
	}

	catch (const PlayerNotFound &e) {

		response.put("status", Response::STATUS_NOT_FOUND);
		response.put("message", PLAYER_NOT_FOUND);

		return response;
	}

	catch (trogdor::Exception &e) {

		response.put("status", Response::STATUS_INTERNAL_ERROR);
		response.put("message", e.what());

		return response;
	}
}

/*****************************************************************************/

JSONObject PlayerController::postInput(JSONObject request) {

	JSONObject response;

	size_t gameId;
	trogdor::entity::Entity *ePtr;
	std::string entityName;

	std::optional<JSONObject> error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr,
		MISSING_PLAYER_NAME,
		INVALID_PLAYER_NAME,
		PLAYER_NOT_FOUND
	);

	if (error.has_value()) {
		return *error;
	}

	boost::optional command = request.get_optional<std::string>("args.command");

	if (!command) {
		response.put("status", Response::STATUS_INVALID);
		response.put("message", MISSING_COMMAND);
	}

	else {
		static_cast<trogdor::entity::Player *>(ePtr)->input(*command);
		response.put("status", Response::STATUS_SUCCESS);
	}

	return response;
}
