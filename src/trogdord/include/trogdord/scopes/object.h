#ifndef SCOPE_OBJECT_H
#define SCOPE_OBJECT_H


#include <trogdor/entities/object.h>
#include "thing.h"


class ObjectController: public ThingController {

	private:

		// Singleton instance of ObjectController
		static std::unique_ptr<ObjectController> instance;

	protected:

		// Converts a object to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

		// Returns a pointer to the object of the specified name. Throws an
		// instance of ObjectNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all object pointers in the game.
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		ObjectController();
		ObjectController(const ObjectController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "object";

		// Returns singleton instance of ObjectController.
		static std::unique_ptr<ObjectController> &get();

		// Returns the scope's name
		virtual const char *getName();
};


#endif
