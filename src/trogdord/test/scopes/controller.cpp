#include <doctest.h>
#include "../mock/mockscope.h"

#include <trogdord/json.h>
#include <trogdord/request.h>
#include <trogdord/response.h>


TEST_SUITE("Controller (scopes/controller.cpp)") {

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with no action and no default action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal(rapidjson::kObjectType);

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", rapidjson::StringRef(methodStr.c_str()), request.GetAllocator());
		request.AddMember("scope", rapidjson::StringRef(scopeStr.c_str()), request.GetAllocator());
		request.AddMember("action", rapidjson::StringRef(actionStr.c_str()), request.GetAllocator());

		rapidjson::Document response = scope->resolve(dummyConnection, methodStr, actionStr, request);

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

		std::string message = std::string("no default action for method ") + methodStr;

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("message"));
		CHECK(response["message"].IsString());
		CHECK(0 == message.compare(response["message"].GetString()));
	}

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with no action and default action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, ScopeController::DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal(rapidjson::kObjectType);

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", rapidjson::StringRef(methodStr.c_str()), request.GetAllocator());
		request.AddMember("scope", rapidjson::StringRef(scopeStr.c_str()), request.GetAllocator());
		request.AddMember("action", rapidjson::StringRef(actionStr.c_str()), request.GetAllocator());

		rapidjson::Document response = scope->resolve(dummyConnection, methodStr, actionStr, request);

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());
	}

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with invalid action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "invalid";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal(rapidjson::kObjectType);

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", rapidjson::StringRef(methodStr.c_str()), request.GetAllocator());
		request.AddMember("scope", rapidjson::StringRef(scopeStr.c_str()), request.GetAllocator());
		request.AddMember("action", rapidjson::StringRef(actionStr.c_str()), request.GetAllocator());

		rapidjson::Document response = scope->resolve(dummyConnection, methodStr, actionStr, request);

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("message"));
		CHECK(response["message"].IsString());
		CHECK(0 == std::string(ScopeController::ACTION_NOT_FOUND).compare(response["message"].GetString()));
	}

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with valid action") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "test";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal(rapidjson::kObjectType);

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", rapidjson::StringRef(methodStr.c_str()), request.GetAllocator());
		request.AddMember("scope", rapidjson::StringRef(scopeStr.c_str()), request.GetAllocator());
		request.AddMember("action", rapidjson::StringRef(actionStr.c_str()), request.GetAllocator());

		rapidjson::Document response = scope->resolve(dummyConnection, methodStr, actionStr, request);

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());
	}

	TEST_CASE("Controller (scopes/controller.cpp): resolve() with valid action but invalid arguments") {

		std::string methodStr = "get";
		std::string scopeStr = "test";
		std::string actionStr = "test";

		auto scope = MockScopeController::factory();
		std::shared_ptr<TCPConnection> dummyConnection = nullptr;

		scope->registerAction(Request::GET, "test", [&] (const rapidjson::Document &request) -> rapidjson::Document {

			rapidjson::Document retVal(rapidjson::kObjectType);

			retVal.AddMember("status", Response::STATUS_SUCCESS, retVal.GetAllocator());
			return retVal;
		});

		rapidjson::Document request(rapidjson::kObjectType);
		rapidjson::Value args(rapidjson::kArrayType);

		request.AddMember("method", rapidjson::StringRef(methodStr.c_str()), request.GetAllocator());
		request.AddMember("scope", rapidjson::StringRef(scopeStr.c_str()), request.GetAllocator());
		request.AddMember("action", rapidjson::StringRef(actionStr.c_str()), request.GetAllocator());
		request.AddMember("args", args, request.GetAllocator());

		rapidjson::Document response = scope->resolve(dummyConnection, methodStr, actionStr, request);

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_INVALID == response["status"].GetUint());

		CHECK(!response.HasParseError());
		CHECK(response.HasMember("message"));
		CHECK(response["message"].IsString());
		CHECK(0 == std::string(ScopeController::INVALID_ARGUMENTS).compare(response["message"].GetString()));
	}
}
