#ifndef SCOPE_GAME_H
#define SCOPE_GAME_H


#include <variant>
#include "controller.h"


class GameController: public ScopeController {

	protected:

		// Singleton instance of GameController
		static std::unique_ptr<GameController> instance;

		// Actions served by the "game" scope
		static const char *LIST_ACTION;
		static const char *DEFINITIONS_ACTION;

		// Error messages
		static const char *MISSING_REQUIRED_NAME;
		static const char *MISSING_REQUIRED_DEFINITION;
		static const char *DEFINITION_NOT_RELATIVE;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		GameController();
		GameController(const GameController &) = delete;

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of GameController.
		static std::unique_ptr<GameController> &get();

		// Returns details about a running game
		JSONObject getGame(JSONObject request);

		// Returns a list of all currently existing games
		JSONObject getGameList(JSONObject request);

		// Returns a list of all available game definitions
		JSONObject getDefinitionList(JSONObject request);

		// Instantiates a new game and returns its id
		JSONObject createGame(JSONObject request);

		// Destroy's a game
		JSONObject destroyGame(JSONObject request);
};


#endif
