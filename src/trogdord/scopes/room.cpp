#include <trogdor/game.h>

#include "../include/scopes/room.h"
#include "../include/exception/entity/roomnotfound.h"


// Scope name that should be used in requests
const char *RoomController::SCOPE = "room";

// Singleton instance of RoomController
std::unique_ptr<RoomController> RoomController::instance = nullptr;

/*****************************************************************************/

JSONObject RoomController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject room = PlaceController::entityToJSONObject(ePtr);

	// TODO: add room-specific properties
	return room;
}

/*****************************************************************************/

trogdor::entity::Entity *RoomController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getRoom(entityName);

	if (!ePtr) {
		throw RoomNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> RoomController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> rooms;

	for (const auto &room: game->getRooms()) {
		rooms.push_back(room.second.get());
	}

	return rooms;
}

/*****************************************************************************/

RoomController::RoomController(): PlaceController() {

}

/*****************************************************************************/

std::unique_ptr<RoomController> &RoomController::get() {

	if (!instance) {
		instance = std::unique_ptr<RoomController>(new RoomController());
	}

	return instance;
}
