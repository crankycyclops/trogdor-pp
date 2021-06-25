#ifndef SCOPE_THING_H
#define SCOPE_THING_H


#include <trogdor/entities/thing.h>
#include "tangible.h"


class ThingController: public TangibleController {

	private:

		// Singleton instance of ThingController
		static std::unique_ptr<ThingController> instance;

	protected:

		// Converts a thing to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

		// Returns a pointer to the thing of the specified name. Throws an
		// instance of ThingNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all thing pointers in the game.
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		ThingController();
		ThingController(const ThingController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "thing";

		// Returns singleton instance of ThingController.
		static std::unique_ptr<ThingController> &get();

		// Returns the scope's name
		virtual const char *getName();
};


#endif
