#ifndef SCOPE_GAME_H
#define SCOPE_GAME_H


#include <variant>
#include "controller.h"


class GameController: public ScopeController {

	protected:

		// Singleton instance of GameController
		static std::unique_ptr<GameController> instance;

		// Actions served by the "game" scope
		static const char *STATISTICS_ACTION;
		static const char *LIST_ACTION;
		static const char *META_ACTION;
		static const char *DEFINITIONS_ACTION;
		static const char *START_ACTION;
		static const char *STOP_ACTION;
		static const char *TIME_ACTION;
		static const char *IS_RUNNING_ACTION;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		GameController();
		GameController(const GameController &) = delete;

	public:

		// Error messages
		static const char *INVALID_ARGUMENTS;
		static const char *INVALID_NAME;
		static const char *MISSING_REQUIRED_NAME;
		static const char *INVALID_DEFINITION;
		static const char *MISSING_REQUIRED_DEFINITION;
		static const char *DEFINITION_NOT_RELATIVE;
		static const char *MISSING_META;
		static const char *INVALID_META;
		static const char *INVALID_META_KEYS;

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of GameController.
		static std::unique_ptr<GameController> &get();

		// Returns details about a running game
		rapidjson::Document getGame(const rapidjson::Document &request);

		// Returns a list of all currently existing games
		rapidjson::Document getGameList(const rapidjson::Document &request);

		// Returns a list of all available game definitions
		rapidjson::Document getDefinitionList(const rapidjson::Document &request);

		// Returns all per-game statistics for a given game
		rapidjson::Document getStatistics(const rapidjson::Document &request);

		// Instantiates a new game and returns its id
		rapidjson::Document createGame(const rapidjson::Document &request);

		// Destroys a game
		rapidjson::Document destroyGame(const rapidjson::Document &request);

		// Starts a game
		rapidjson::Document startGame(const rapidjson::Document &request);

		// Stops a game
		rapidjson::Document stopGame(const rapidjson::Document &request);

		// Returns the requested meta values
		rapidjson::Document getMeta(const rapidjson::Document &request);

		// Sets the requested meta values
		rapidjson::Document setMeta(const rapidjson::Document &request);

		// Returns the current game time
		rapidjson::Document getTime(const rapidjson::Document &request);

		// Returns whether or not the game is currently running
		rapidjson::Document getIsRunning(const rapidjson::Document &request);
};


#endif
