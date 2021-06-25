#include <trogdor/game.h>

#include <trogdord/scopes/room.h>
#include <trogdord/exception/entity/roomnotfound.h>


// Singleton instance of RoomController
std::unique_ptr<RoomController> RoomController::instance;

/*****************************************************************************/

rapidjson::Value RoomController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value room = PlaceController::entityToJSONObject(ePtr, allocator);

	// TODO: add room-specific properties
	return room;
}

/*****************************************************************************/

trogdor::entity::Entity *RoomController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getRoom(entityName).get();

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

/*****************************************************************************/

const char *RoomController::getName() {

	return SCOPE;
}
