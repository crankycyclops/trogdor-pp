#ifndef SCOPE_BEING_H
#define SCOPE_BEING_H


#include <trogdor/entities/being.h>
#include "thing.h"


class BeingController: public ThingController {

	private:

		// Singleton instance of BeingController
		static std::unique_ptr<BeingController> instance;

	protected:

		// Converts a being to a JSON object
		virtual JSONObject entityToJSONObject(trogdor::entity::Entity *ePtr);

		// Returns a pointer to the being of the specified name. Throws an
		// instance of BeingNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all being pointers in the game.
		virtual const trogdor::entity::EntityMap getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		BeingController();
		BeingController(const BeingController &) = delete;

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of BeingController.
		static std::unique_ptr<BeingController> &get();
};


#endif
