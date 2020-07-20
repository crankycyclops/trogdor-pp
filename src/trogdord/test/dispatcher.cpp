#include <doctest.h>

#include "mock/mockscope.h"
#include "mock/mockdispatcher.h"

#include "../include/json.h"
#include "../include/request.h"
#include "../include/response.h"


TEST_SUITE("Dispatcher (dispatcher.cpp") {

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is empty string") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		JSONObject response = JSON::deserialize(dispatcher.dispatch(dummyConnection, ""));
		CHECK(Response::STATUS_INVALID == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is invalid JSON") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		JSONObject response = JSON::deserialize(dispatcher.dispatch(dummyConnection, "I'm not valid JSON"));
		CHECK(Response::STATUS_INVALID == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is empty JSON object") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		JSONObject response = JSON::deserialize(dispatcher.dispatch(dummyConnection, "{}"));
		CHECK(Response::STATUS_INVALID == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing required method") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		JSONObject response = JSON::deserialize(dispatcher.dispatch(dummyConnection, "{\"scope\":\"global\",\"action\":\"statistics\"}"));
		CHECK(Response::STATUS_INVALID == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing required scope") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		JSONObject response = JSON::deserialize(dispatcher.dispatch(dummyConnection, "{\"method\":\"get\",\"action\":\"statistics\"}"));
		CHECK(Response::STATUS_INVALID == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing action for scope with no default") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		JSONObject response = JSON::deserialize(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\"}"
		));

		CHECK(Response::STATUS_NOT_FOUND == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing action for scope with default") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, ScopeController::DEFAULT_ACTION, [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		JSONObject response = JSON::deserialize(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\"}"
		));

		CHECK(Response::STATUS_SUCCESS == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid method") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		JSONObject response = JSON::deserialize(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"invalid\",\"scope\":\"test\",\"action\":\"test\"}"
		));

		CHECK(Response::STATUS_NOT_FOUND == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid scope") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		JSONObject response = JSON::deserialize(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"invalid\",\"action\":\"test\"}"
		));

		CHECK(Response::STATUS_NOT_FOUND == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid action") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		JSONObject response = JSON::deserialize(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\",\"action\":\"invalid\"}"
		));

		CHECK(Response::STATUS_NOT_FOUND == response.get<size_t>("status"));
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid and routes to existing method, scope, and action") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (JSONObject request) -> JSONObject {

			JSONObject retVal;

			retVal.put("status", Response::STATUS_SUCCESS);
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		JSONObject response = JSON::deserialize(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\",\"action\":\"test\"}"
		));

		CHECK(Response::STATUS_SUCCESS == response.get<size_t>("status"));
	}
}
