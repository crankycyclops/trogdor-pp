#ifndef SCOPE_PLAYER_H
#define SCOPE_PLAYER_H


#include <trogdor/entities/player.h>
#include "entity.h"


class PlayerController: public EntityController {

	private:

		// Singleton instance of PlayerController
		static std::unique_ptr<PlayerController> instance;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		PlayerController();
		PlayerController(const PlayerController &) = delete;

	protected:

		// Actions served by the "player" scope
		static const char *LIST_ACTION;

		// Error messages
		static const char *MISSING_PLAYER_NAME;
		static const char *INVALID_PLAYER_NAME;
		static const char *PLAYER_NOT_FOUND;

		// Returns a pointer to the player of the specified name. Throws an
		// instance of PlayerNotFound if the player doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all player pointers in the game.
		virtual const trogdor::entity::EntityMap getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of PlayerController.
		static std::unique_ptr<PlayerController> &get();

		// Creates a player in the specified game
		JSONObject createPlayer(JSONObject request);

		// Removes a player in the specified game
		JSONObject destroyPlayer(JSONObject request);
};


#endif
