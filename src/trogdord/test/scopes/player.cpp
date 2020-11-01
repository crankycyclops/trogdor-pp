#include <doctest.h>
#include "helper.h"


// Test game and player names
static const char *gameName = "myGame";
static const char *playerName = "player";

TEST_SUITE("PlayerController (scopes/player.cpp)") {

	TEST_CASE("PlayerController (scopes/player.cpp): getEntity()") {

		GameContainer::get()->reset();

		initGameXML();
		initConfig();

		rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

		CHECK(trogdor::isAscii(JSON::serialize(response)));

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("id"));
		CHECK(response["id"].IsUint());

		size_t gameId = response["id"].GetUint();

		// Make sure we have at least one entity of each type in the game
		response = createPlayer(gameId, playerName);

		CHECK(trogdor::isAscii(JSON::serialize(response)));

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		response = getEntity(gameId, playerName, "player");

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("entity"));
		CHECK(response["entity"].IsObject());

		CHECK(response["entity"].HasMember("name"));
		CHECK(response["entity"]["name"].GetString());
		CHECK(0 == std::string(playerName).compare(response["entity"]["name"].GetString()));

		CHECK(response["entity"].HasMember("type"));
		CHECK(response["entity"]["type"].GetString());
		CHECK(0 == std::string("player").compare(response["entity"]["name"].GetString()));

		destroyGameXML();
		destroyConfig();
	}

	TEST_CASE("PlayerController (scopes/player.cpp): getEntityList()") {

		GameContainer::get()->reset();

		initGameXML();
		initConfig();

		rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

		CHECK(trogdor::isAscii(JSON::serialize(response)));

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("id"));
		CHECK(response["id"].IsUint());

		size_t gameId = response["id"].GetUint();

		// Make sure we have at least one entity of each type in the game
		response = createPlayer(gameId, playerName);

		CHECK(trogdor::isAscii(JSON::serialize(response)));

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		response = getEntityList(gameId, "player");

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("entities"));
		CHECK(response["entities"].IsArray());
		CHECK(1 == response["entities"].Size());

		response["entities"][0].IsObject();

		CHECK(response["entities"][0].HasMember("name"));
		CHECK(response["entities"][0]["name"].IsString());
		CHECK(0 == std::string(playerName).compare(response["entities"][0]["name"].GetString()));

		CHECK(response["entities"][0].HasMember("type"));
		CHECK(response["entities"][0]["type"].IsString());

		destroyGameXML();
		destroyConfig();
	}
}
