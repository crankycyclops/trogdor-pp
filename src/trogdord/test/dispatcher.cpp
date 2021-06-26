#include <doctest.h>

#include <trogdord/json.h>
#include <trogdord/request.h>
#include <trogdord/response.h>

#include "mock/mockscope.h"
#include "mock/mockdispatcher.h"


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

			rapidjson::Document retVal(rapidjson::kObjectType);

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

			rapidjson::Document retVal(rapidjson::kObjectType);

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

			rapidjson::Document retVal(rapidjson::kObjectType);

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

			rapidjson::Document retVal(rapidjson::kObjectType);

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

			rapidjson::Document retVal(rapidjson::kObjectType);

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

			rapidjson::Document retVal(rapidjson::kObjectType);

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

	TEST_CASE("Dispatcher (dispatcher.cpp): registerScope() and unregisterScope()") {

		auto scopeTest = MockScopeController::factory();
		auto scopeTest2 = MockScopeController::factory();

		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scopeTest->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal(rapidjson::kObjectType);

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		// The different status returned will identify the second test scope
		// when I make a request for it
		scopeTest2->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal(rapidjson::kObjectType);

			retVal.AddMember("status", Response::STATUS_PARTIAL_CONTENT, retVal.GetAllocator());
			return retVal;
		});

		// Start out with just one scope registered
		MockDispatcher dispatcher({
			{"test1", scopeTest.get()}
		});

		rapidjson::Document response;

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test1\",\"action\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		// Try calling the second scope before it's registered and verify that it's not found
		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test2\",\"action\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

		// Now, register the second scope and show it can now be found
		dispatcher.registerScope(scopeTest2.get());

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"mock\",\"action\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_PARTIAL_CONTENT == response["status"].GetUint());

		// Finally, unregister it and show it's not found again
		dispatcher.unregisterScope(scopeTest2->getName());

		response.Parse(dispatcher.dispatch(
			dummyConnection,
			"{\"method\":\"get\",\"scope\":\"test2\",\"action\":\"test\"}"
		).c_str());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());
	}
}
