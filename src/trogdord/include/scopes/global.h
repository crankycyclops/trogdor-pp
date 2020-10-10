#ifndef SCOPE_GLOBAL_H
#define SCOPE_GLOBAL_H


#include "controller.h"


class GlobalController: public ScopeController {

	protected:

		// Singleton instance of GlobalController
		static std::unique_ptr<GlobalController> instance;

		// Action names that get mapped to methods in GlobalController
		static const char *STATISTICS_ACTION;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		GlobalController();
		GlobalController(const GlobalController &) = delete;

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of GlobalController.
		static std::unique_ptr<GlobalController> &get();

		// Action that returns statistical information about the server
		rapidjson::Document statistics(const rapidjson::Document &request);
};


#endif
