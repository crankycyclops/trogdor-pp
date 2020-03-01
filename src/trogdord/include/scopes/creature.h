#ifndef SCOPE_CREATURE_H
#define SCOPE_CREATURE_H


#include <trogdor/entities/creature.h>
#include "being.h"


class CreatureController: public BeingController {

	private:

		// Singleton instance of CreatureController
		static std::unique_ptr<CreatureController> instance;

	protected:

		// Converts a creature to a JSON object
		virtual JSONObject entityToJSONObject(trogdor::entity::Entity *ePtr);

		// Returns a pointer to the creature of the specified name. Throws an
		// instance of CreatureNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all creature pointers in the game.
		virtual const trogdor::entity::EntityMap getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		CreatureController();
		CreatureController(const CreatureController &) = delete;

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of CreatureController.
		static std::unique_ptr<CreatureController> &get();
};


#endif
