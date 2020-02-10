#ifndef SCOPE_GLOBAL_H
#define SCOPE_GLOBAL_H


#include "controller.h"


class Global: public ScopeController {

	protected:

		// Singleton instance of Global
		static std::unique_ptr<Global> instance;

		// Action names that get mapped to methods in Global
		static const char *STATISTICS_ACTION;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		Global();
		Global(const Global &) = delete;

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of Global.
		static std::unique_ptr<Global> &get();

		// Action that returns statistical information about the server
		JSONObject statistics(JSONObject request);
};


#endif
