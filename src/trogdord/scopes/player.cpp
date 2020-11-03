#include <rapidjson/pointer.h>
#include <trogdor/exception/duplicateentity.h>

#include "../include/request.h"
#include "../include/gamecontainer.h"

#include "../include/scopes/player.h"
#include "../include/exception/entity/playernotfound.h"


// Scope name that should be used in requests
const char *PlayerController::SCOPE = "player";

// Actions served by the "Player" scope
const char *PlayerController::INPUT_ACTION = "input";

// Error messages
const char *PlayerController::MISSING_PLAYER_NAME = "missing required player name";
const char *PlayerController::INVALID_PLAYER_NAME = "invalid player name";
const char *PlayerController::INVALID_COMMAND = "command must be a string";
const char *PlayerController::PLAYER_NOT_FOUND = "player not found";

// Singleton instance of PlayerController
std::unique_ptr<PlayerController> PlayerController::instance;

/*****************************************************************************/

rapidjson::Value PlayerController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value player = BeingController::entityToJSONObject(ePtr, allocator);

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

	registerAction(Request::POST, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->createPlayer(request);
	});

	registerAction(Request::DELETE, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->destroyPlayer(request);
	});

	registerAction(Request::POST, INPUT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
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

rapidjson::Document PlayerController::createPlayer(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	size_t gameId;
	std::string playerName;

	try {
		gameId = Request::parseGameId(request, "/args/game_id");
		playerName = Request::parseArgumentString(
			request,
			"/args/name",
			MISSING_PLAYER_NAME,
			INVALID_PLAYER_NAME
		);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	if (!trogdor::entity::Entity::isNameValid(playerName)) {

		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(INVALID_PLAYER_NAME), response.GetAllocator());

		return response;
	}

	try {

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("player", entityToJSONObject(
			GameContainer::get()->createPlayer(gameId, playerName),
			response.GetAllocator()
		), response.GetAllocator());

		return response;
	}

	catch (const GameNotFound &e) {

		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());

		return response;
	}

	catch (const trogdor::entity::DuplicateEntity &e) {

		response.AddMember("status", Response::STATUS_CONFLICT, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(e.what()), response.GetAllocator());

		return response;
	}

	catch (const trogdor::Exception &e) {

		response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(e.what()), response.GetAllocator());

		return response;
	}
}

/*****************************************************************************/

rapidjson::Document PlayerController::destroyPlayer(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	size_t gameId;
	std::string playerName;

	// If a removal message is sent as part of the request, it will be sent to
	// the player's notifications channel before they're removed from the game.
	std::string removalMessage = "";

	const rapidjson::Value *messageArg = rapidjson::Pointer("/args/message").Get(request);

	if (messageArg) {

		std::optional<std::string> mval = JSON::valueToStr(*messageArg);

		if (mval) {
			removalMessage = *mval;
		}
	}

	try {
		gameId = Request::parseGameId(request, "/args/game_id");
		playerName = Request::parseArgumentString(
			request,
			"/args/name",
			MISSING_PLAYER_NAME,
			INVALID_PLAYER_NAME
		);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	try {

		GameContainer::get()->removePlayer(gameId, playerName, removalMessage);
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());

		return response;
	}

	catch (const GameNotFound &e) {

		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());

		return response;
	}

	catch (const PlayerNotFound &e) {

		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(PLAYER_NOT_FOUND), response.GetAllocator());

		return response;
	}

	catch (trogdor::Exception &e) {

		response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(e.what()), response.GetAllocator());

		return response;
	}
}

/*****************************************************************************/

rapidjson::Document PlayerController::postInput(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	size_t gameId;
	trogdor::entity::Entity *ePtr;
	std::string entityName;

	rapidjson::Document error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr,
		MISSING_PLAYER_NAME,
		INVALID_PLAYER_NAME,
		PLAYER_NOT_FOUND
	);

	if (error.Size()) {
		return error;
	}

	const rapidjson::Value *command = rapidjson::Pointer("/args/command").Get(request);

	if (!command) {
		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(MISSING_COMMAND), response.GetAllocator());
	}

	else if (!command->IsString()) {
		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(INVALID_COMMAND), response.GetAllocator());
	}

	else {
		static_cast<trogdor::entity::Player *>(ePtr)->input(command->GetString());
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	}

	return response;
}
