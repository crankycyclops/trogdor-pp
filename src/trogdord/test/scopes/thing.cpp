#include <doctest.h>
#include "helper.h"


// Test game and player names
static const char *gameName = "myGame";
static const char *playerName = "player";

TEST_SUITE("ThingController (scopes/thing.cpp)") {

	TEST_CASE("ThingController (scopes/thing.cpp): getEntity()") {

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

		for (const auto &entity: entities) {

			response = getEntity(gameId, entity.c_str(), "thing");

			if (things.end() != things.find(entity)) {

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				CHECK(response.HasMember("entity"));
				CHECK(response["entity"].IsObject());

				CHECK(response["entity"].HasMember("name"));
				CHECK(response["entity"]["name"].GetString());
				CHECK(0 == entity.compare(response["entity"]["name"].GetString()));

				CHECK(response["entity"].HasMember("type"));
				CHECK(response["entity"]["type"].GetString());
				CHECK(trogdor::isAscii(response["entity"]["type"].GetString()));
			}

			else {

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

				CHECK(response.HasMember("message"));
				CHECK(response["message"].IsString());
				CHECK(0 == std::string(EntityController::ENTITY_NOT_FOUND).compare(response["message"].GetString()));
			}
		}

		// The player should also be a thing
		response = getEntity(gameId, playerName, "thing");

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

	TEST_CASE("ThingController (scopes/thing.cpp): getEntityList()") {

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

		response = getEntityList(gameId, "thing");

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("entities"));
		CHECK(response["entities"].IsArray());
		CHECK(response["entities"].Size() > 0);

		for (auto i = response["entities"].Begin(); i != response["entities"].End(); i++) {

			CHECK(i->IsObject());

			CHECK(i->HasMember("name"));
			CHECK((*i)["name"].IsString());

			CHECK(i->HasMember("type"));
			CHECK((*i)["type"].IsString());
			CHECK(trogdor::isAscii((*i)["name"].GetString()));

			bool entityValid =
				things.end() != things.find((*i)["name"].GetString()) ||
				0 == std::string(playerName).compare((*i)["name"].GetString());

			CHECK(entityValid);
		}

		destroyGameXML();
		destroyConfig();
	}
}
