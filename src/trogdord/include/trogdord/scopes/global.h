#ifndef SCOPE_GLOBAL_H
#define SCOPE_GLOBAL_H


#include "controller.h"


class GlobalController: public ScopeController {

	protected:

		// Singleton instance of GlobalController
		static std::unique_ptr<GlobalController> instance;

		// Action names that get mapped to methods in GlobalController
		static constexpr const char *CONFIG_ACTION = "config";
		static constexpr const char *STATISTICS_ACTION = "statistics";
		static constexpr const char *DUMP_ACTION = "dump";
		static constexpr const char *RESTORE_ACTION = "restore";

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		GlobalController();
		GlobalController(const GlobalController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "global";

		// Returns singleton instance of GlobalController.
		static std::unique_ptr<GlobalController> &get();

		// Returns the scope's name
		virtual const char *getName();

		// Action that returns non-sensitive trogdord.ini settings
		rapidjson::Document config(const rapidjson::Document &request);

		// Action that returns statistical information about the server
		rapidjson::Document statistics(const rapidjson::Document &request);

		// Action that dumps the server's current state (games and all) to disk
		rapidjson::Document dump(const rapidjson::Document &request);

		// Action that restores the server's current state (games and all) from disk
		rapidjson::Document restore(const rapidjson::Document &request);
};


#endif
