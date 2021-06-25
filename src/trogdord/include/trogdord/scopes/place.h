#ifndef SCOPE_PLACE_H
#define SCOPE_PLACE_H


#include <trogdor/entities/place.h>
#include "tangible.h"


class PlaceController: public TangibleController {

	private:

		// Singleton instance of PlaceController
		static std::unique_ptr<PlaceController> instance;

	protected:

		// Converts a place to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

		// Returns a pointer to the place of the specified name. Throws an
		// instance of PlaceNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all place pointers in the game.
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		PlaceController();
		PlaceController(const PlaceController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "place";

		// Returns singleton instance of PlaceController.
		static std::unique_ptr<PlaceController> &get();

		// Returns the scope's name
		virtual const char *getName();
};


#endif
