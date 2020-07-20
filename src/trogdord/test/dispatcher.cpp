#include <doctest.h>
#include "mock/mockdispatcher.h"

#include "../include/json.h"
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

		// TODO
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing action for scope with default") {

		// TODO
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid method") {

		// TODO
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid scope") {

		// TODO
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid action") {

		// TODO
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid and routes to existing method, scope, and action") {

		// TODO
	}
}
