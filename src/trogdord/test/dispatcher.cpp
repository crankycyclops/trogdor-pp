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

		rapidjson::Document response;
		response.Parse(dispatcher.dispatch(dummyConnection, "").c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_INVALID == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is invalid JSON") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		rapidjson::Document response;
		response.Parse(dispatcher.dispatch(dummyConnection, "I'm not valid JSON").c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_INVALID == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is empty JSON object") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		rapidjson::Document response;
		response.Parse(dispatcher.dispatch(dummyConnection, "{}").c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_INVALID == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing required method") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		rapidjson::Document response;
		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"scope\":\"global\",\"action\":\"statistics\"}").c_str()
		);

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_INVALID == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing required scope") {

		MockDispatcher dispatcher;
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		rapidjson::Document response;
		response.Parse(
			dispatcher.dispatch(dummyConnection,
			"{\"method\":\"get\",\"action\":\"statistics\"}").c_str()
		);

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_INVALID == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing action for scope with no default") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal;

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		rapidjson::Document response;

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is missing action for scope with default") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, ScopeController::DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal;

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		rapidjson::Document response;

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid method") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal;

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		rapidjson::Document response;

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"invalid\",\"scope\":\"test\",\"action\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid scope") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal;

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		rapidjson::Document response;

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"invalid\",\"action\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid format but with invalid action") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal;

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		rapidjson::Document response;

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\",\"action\":\"invalid\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());
	}

	TEST_CASE("Dispatcher (dispatcher.cpp): Request is valid and routes to existing method, scope, and action") {

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal;

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		MockDispatcher dispatcher({
			{"test", scope.get()}
		});

		rapidjson::Document response;

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test\",\"action\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());
	}
}
