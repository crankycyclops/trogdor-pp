#ifndef MOCK_SCOPE_H
#define MOCK_SCOPE_H


#include <trogdord/scopes/controller.h>


class MockScopeController: public ScopeController {

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "mock";

		// Returns unique instances of MockScopeController.
		static std::unique_ptr<MockScopeController> factory();

		// Returns the scope's name
		virtual const char *getName();

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
