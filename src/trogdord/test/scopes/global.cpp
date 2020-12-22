#include <doctest.h>
#include <trogdor/version.h>

#include "../../include/json.h"
#include "../../include/request.h"
#include "../../include/response.h"

#include "../../include/version.h"
#include "../../include/scopes/global.h"


TEST_SUITE("GlobalController (scopes/global.cpp)") {

	// TODO: need to add a separate test case for number of games
	TEST_CASE("GlobalController (scopes/global.cpp): resolve() with no action and no default action") {

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", "get", request.GetAllocator());
		request.AddMember("scope", "global", request.GetAllocator());
		request.AddMember("action", "statistics", request.GetAllocator());

		rapidjson::Document response = GlobalController::get()->statistics(request);

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("players"));
		CHECK(response["status"].IsUint());
		CHECK(0 == response["players"].GetUint());

		CHECK(response.HasMember("version"));
		CHECK(response["version"].IsObject());

		CHECK(response["version"].HasMember("major"));
		CHECK(response["version"]["major"].IsUint());
		CHECK(TROGDORD_VERSION_MAJOR == response["version"]["major"].GetUint());

		CHECK(response["version"].HasMember("minor"));
		CHECK(response["version"]["minor"].IsUint());
		CHECK(TROGDORD_VERSION_MINOR == response["version"]["minor"].GetUint());

		CHECK(response["version"].HasMember("patch"));
		CHECK(response["version"]["patch"].IsUint());
		CHECK(TROGDORD_VERSION_PATCH == response["version"]["patch"].GetUint());

		CHECK(response.HasMember("lib_version"));
		CHECK(response["lib_version"].IsObject());

		CHECK(response["lib_version"].HasMember("major"));
		CHECK(response["lib_version"]["major"].IsUint());
		CHECK(TROGDOR_VERSION_MAJOR == response["lib_version"]["major"].GetUint());

		CHECK(response["lib_version"].HasMember("minor"));
		CHECK(response["lib_version"]["minor"].IsUint());
		CHECK(TROGDOR_VERSION_MINOR == response["lib_version"]["minor"].GetUint());

		CHECK(response["lib_version"].HasMember("patch"));
		CHECK(response["lib_version"]["patch"].IsUint());
		CHECK(TROGDOR_VERSION_PATCH == response["lib_version"]["patch"].GetUint());
	}

	TEST_CASE("GlobalController (scopes/global.cpp): dump()") {

		SUBCASE("State disabled, no games") {
			// TODO: should return 501 unsupported
		}

		SUBCASE("State disabled, one game") {
			// TODO: should return 501 unsupported
		}

		SUBCASE("State enabled, no games") {
			// TODO: should return 200 success
		}

		SUBCASE("State enabled, one game") {
			// TODO: should return 200 success
		}
	}

	TEST_CASE("GlobalController (scopes/global.cpp): restore()") {

		SUBCASE("State disabled, no current games, no dumped games") {
			// TODO: should return 501 unsupported, no games should exist
		}

		SUBCASE("State disabled, no current games, one dumped game") {
			// TODO: should return 501 unsupported, no games should exist
		}

		SUBCASE("State disabled, one current games, no dumped games") {
			// TODO: should return 501 unsupported, one game should remain, and it should be the same
		}

		SUBCASE("State disabled, one current games, one dumped game") {
			// TODO: should return 501 unsupported, one game should remain, and it should be the same
		}

		SUBCASE("State enabled, no current games, no dumped games") {
			// TODO: should return 200, no games should exist
		}

		SUBCASE("State enabled, no current games, one dumped game") {
			// TODO: should return 200, have the one previously dumped game
		}

		SUBCASE("State enabled, one current games, no dumped games") {
			// TODO: should return 200, zero games should remain
		}

		SUBCASE("State enabled, one current games, one dumped game") {
			// TODO: should return 200, game should be replaced by previously dumped one
		}
	}
}
