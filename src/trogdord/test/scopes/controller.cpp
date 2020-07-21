#include <doctest.h>
#include "../mock/mockscope.h"

#include "../include/json.h"
#include "../include/request.h"
#include "../include/response.h"


TEST_SUITE("Controller (scopes/controller.cpp)") {

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with no action and no default action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		JSONObject request;

		request.put("method", methodStr);
		request.put("scope", scopeStr);
		request.put("action", actionStr);

		JSONObject response = scope->resolve(dummyConnection, methodStr, actionStr, request);
		CHECK(Response::STATUS_NOT_FOUND == response.get<size_t>("status"));
	}

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with no action and default action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, ScopeController::DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		JSONObject request;

		request.put("method", methodStr);
		request.put("scope", scopeStr);
		request.put("action", actionStr);

		JSONObject response = scope->resolve(dummyConnection, methodStr, actionStr, request);
		CHECK(Response::STATUS_SUCCESS == response.get<size_t>("status"));
	}

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with invalid action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "invalid";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		JSONObject request;

		request.put("method", methodStr);
		request.put("scope", scopeStr);
		request.put("action", actionStr);

		JSONObject response = scope->resolve(dummyConnection, methodStr, actionStr, request);
		CHECK(Response::STATUS_NOT_FOUND == response.get<size_t>("status"));
	}

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with valid action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "test";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		JSONObject request;

		request.put("method", methodStr);
		request.put("scope", scopeStr);
		request.put("action", actionStr);

		JSONObject response = scope->resolve(dummyConnection, methodStr, actionStr, request);
		CHECK(Response::STATUS_SUCCESS == response.get<size_t>("status"));
	}
}
