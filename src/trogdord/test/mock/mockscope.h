#ifndef MOCK_SCOPE_H
#define MOCK_SCOPE_H


#include "../../include/scopes/controller.h"


class MockScopeController: public ScopeController {

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns unique instances of MockScopeController.
		static std::unique_ptr<MockScopeController> factory();

		// Wraps around ScopeController::registerAction() to provide a public
		// interface used for testing.
		inline void registerAction(
			std::string method,
			std::string action,
			std::function<rapidjson::Document(const rapidjson::Document &)> callback
		) {

			ScopeController::registerAction(method, action, callback);
		}
};


#endif
