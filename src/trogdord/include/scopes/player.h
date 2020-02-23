#ifndef SCOPE_PLAYER_H
#define SCOPE_PLAYER_H


#include <trogdor/entities/player.h>
#include "controller.h"


class PlayerController: public ScopeController {

	protected:

		// Singleton instance of PlayerController
		static std::unique_ptr<PlayerController> instance;

		// Actions served by the "player" scope
		static const char *LIST_ACTION;

		// Error messages
		static const char *MISSING_PLAYER_NAME;
		static const char *INVALID_PLAYER_NAME;
		static const char *PLAYER_NOT_FOUND;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		PlayerController();
		PlayerController(const PlayerController &) = delete;

		// Converts a player to a JSON object
		JSONObject playerToJSONObject(trogdor::entity::Player *pPtr);

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of PlayerController.
		static std::unique_ptr<PlayerController> &get();

		// Returns details about the player in the specified game
		JSONObject getPlayer(JSONObject request);

		// Returns a list of all players in the specified game
		JSONObject getPlayerList(JSONObject request);

		// Creates a player in the specified game
		JSONObject createPlayer(JSONObject request);

		// Removes a player in the specified game
		JSONObject destroyPlayer(JSONObject request);
};


#endif
