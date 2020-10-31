#include <doctest.h>
#include "helper.h"


// Test game and player names
static const char *gameName = "myGame";
static const char *playerName = "player";

TEST_SUITE("TangibleController (scopes/tangible.cpp)") {

	TEST_CASE("TangibleController (scopes/tangible.cpp): getEntity()") {

		// TODO: create a game and verify that we can retrieve tangible
		// entities but not other entities. Should create a player so that I
		// have at least one entity of each type in the game.
	}

	TEST_CASE("TangibleController (scopes/tangible.cpp): getEntityList()") {

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

		response = getEntityList(gameId, "tangible");

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("entities"));
		CHECK(response["entities"].IsArray());
		CHECK(response["entities"].Size() > 0);

		for (auto i = response["entities"].Begin(); i != response["entities"].End(); i++) {

			std::cout << (*i)["name"].GetString() << std::endl;

			CHECK(i->IsObject());

			CHECK(i->HasMember("name"));
			CHECK((*i)["name"].IsString());

			CHECK(i->HasMember("type"));
			CHECK((*i)["type"].IsString());
			CHECK(trogdor::isAscii((*i)["name"].GetString()));

			bool entityValid =
				tangibles.end() != tangibles.find((*i)["name"].GetString()) ||
				0 == std::string(playerName).compare((*i)["name"].GetString());

			CHECK(entityValid);
		}
	}
}
