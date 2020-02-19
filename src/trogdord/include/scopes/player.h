#ifndef SCOPE_PLAYER_H
#define SCOPE_PLAYER_H


#include "controller.h"


class PlayerController: public ScopeController {

	protected:

		// Singleton instance of PlayerController
		static std::unique_ptr<PlayerController> instance;

		// Actions served by the "player" scope
		static const char *LIST_ACTION;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		PlayerController();
		PlayerController(const PlayerController &) = delete;

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of PlayerController.
		static std::unique_ptr<PlayerController> &get();

		// Returns a list of all players in the specified game
		JSONObject getPlayerList(JSONObject request);
};


#endif
