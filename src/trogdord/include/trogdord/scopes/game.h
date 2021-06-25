#ifndef SCOPE_GAME_H
#define SCOPE_GAME_H


#include <variant>
#include "controller.h"


class GameController: public ScopeController {

	protected:

		// Singleton instance of GameController
		static std::unique_ptr<GameController> instance;

		// Actions served by the "game" scope
		static constexpr const char *STATISTICS_ACTION = "statistics";
		static constexpr const char *LIST_ACTION = "list";
		static constexpr const char *META_ACTION = "meta";
		static constexpr const char *DEFINITIONS_ACTION = "definitions";
		static constexpr const char *START_ACTION = "start";
		static constexpr const char *STOP_ACTION = "stop";
		static constexpr const char *TIME_ACTION = "time";
		static constexpr const char *IS_RUNNING_ACTION = "is_running";
		static constexpr const char *DUMP_LIST_ACTION = "dumplist";
		static constexpr const char *DUMP_ACTION = "dump";
		static constexpr const char *RESTORE_ACTION = "restore";

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		GameController();
		GameController(const GameController &) = delete;

	public:

		// Error messages
		static constexpr const char *INVALID_NAME = "game name must be a string";
		static constexpr const char *MISSING_REQUIRED_NAME = "missing required name";
		static constexpr const char *INVALID_DEFINITION = "game definition filename must be a string";
		static constexpr const char *MISSING_REQUIRED_DEFINITION = "missing required definition path";
		static constexpr const char *DEFINITION_NOT_RELATIVE = "definition path must be relative";
		static constexpr const char *MISSING_META = "missing required meta key, value pairs";
		static constexpr const char *INVALID_META = "meta values cannot be objects or arrays";
		static constexpr const char *INVALID_META_KEYS = "invalid meta keys";
		static constexpr const char *INVALID_FILTER_ARG = "filters must be expressed as a JSON object or array";
		static constexpr const char *INVALID_DELETE_DUMP_ARG = "delete_dump must be a boolean value";
		static constexpr const char *DUMPED_GAME_NOT_FOUND = "dumped game not found";
		static constexpr const char *INVALID_DUMPED_GAME_SLOT = "game slot must be an unsigned integer";
		static constexpr const char *DUMPED_GAME_SLOT_NOT_FOUND = "game slot not found";

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "game";

		// Returns singleton instance of GameController.
		static std::unique_ptr<GameController> &get();

		// Returns the scope's name
		virtual const char *getName();

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

		// Destroys a dumped game or slot
		rapidjson::Document destroyDump(const rapidjson::Document &request);

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

		// Returns the details of a dumped game or a specific slot
		rapidjson::Document getDump(const rapidjson::Document &request);

		// Returns a list of either dumped game ids or save slots for a
		// specific game, depending on the arguments
		rapidjson::Document getDumpList(const rapidjson::Document &request);

		// Dumps the specified game to disk
		rapidjson::Document dumpGame(const rapidjson::Document &request);

		// Restores the specified game from disk
		rapidjson::Document restoreGame(const rapidjson::Document &request);
};


#endif
