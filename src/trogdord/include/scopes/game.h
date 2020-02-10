#ifndef SCOPE_GAME_H
#define SCOPE_GAME_H


#include "controller.h"


class Game: public ScopeController {

	protected:

		// Singleton instance of Game
		static std::unique_ptr<Game> instance;

		// Error messages
		static const char *MISSING_GAME_ID;
		static const char *INVALID_GAME_ID;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		Game();
		Game(const Game &) = delete;

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of Game.
		static std::unique_ptr<Game> &get();

		// Returns details about a running game
		JSONObject getGame(JSONObject request);

		// Instantiates a new game and returns its id
		JSONObject createGame(JSONObject request);
};


#endif
