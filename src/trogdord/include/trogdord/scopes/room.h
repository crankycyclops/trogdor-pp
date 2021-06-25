#ifndef SCOPE_ROOM_H
#define SCOPE_ROOM_H


#include <trogdor/entities/room.h>
#include "place.h"


class RoomController: public PlaceController {

	private:

		// Singleton instance of RoomController
		static std::unique_ptr<RoomController> instance;

	protected:

		// Converts a room to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

		// Returns a pointer to the room of the specified name. Throws an
		// instance of RoomNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all room pointers in the game.
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		RoomController();
		RoomController(const RoomController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "room";

		// Returns singleton instance of RoomController.
		static std::unique_ptr<RoomController> &get();

		// Returns the scope's name
		virtual const char *getName();
};


#endif
