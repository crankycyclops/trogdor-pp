#include <doctest.h>
#include "helper.h"

using trogdor::entity::Entity;


// Test game name
static const char *gameName = "myGame";

TEST_SUITE("EntityController (scopes/entity.cpp)") {

	TEST_CASE("EntityController (scopes/entity.cpp): getEntity()") {

		SUBCASE("No args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getEntity(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Empty args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getEntity(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("name", "trogdor", request.GetAllocator());

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getEntity(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing entity name") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getEntity(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_ENTITY_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, invalid game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getEntity(0, "test");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, invalid game id") {

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

			response = getEntity(gameId + 1, "test");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, invalid entity name") {

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

			response = getEntity(gameId, "NotAnEntity");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::ENTITY_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Stopped game, valid game id and entity name") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			const char *entityName = "start";
			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t gameId = response["id"].GetUint();

			response = stopGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntity(gameId, entityName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("entity"));
			CHECK(response["entity"].IsObject());

			CHECK(response["entity"].HasMember("name"));
			CHECK(response["entity"]["name"].IsString());
			CHECK(0 == std::string(entityName).compare(response["entity"]["name"].GetString()));

			CHECK(response["entity"].HasMember("type"));
			CHECK(response["entity"]["type"].IsString());
			CHECK(0 == std::string("room").compare(response["entity"]["type"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Started game, valid game id and entity name") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			const char *entityName = "start";
			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t gameId = response["id"].GetUint();

			response = startGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntity(gameId, entityName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("entity"));
			CHECK(response["entity"].IsObject());

			CHECK(response["entity"].HasMember("name"));
			CHECK(response["entity"]["name"].IsString());
			CHECK(0 == std::string(entityName).compare(response["entity"]["name"].GetString()));

			CHECK(response["entity"].HasMember("type"));
			CHECK(response["entity"]["type"].IsString());
			CHECK(0 == std::string("room").compare(response["entity"]["type"].GetString()));

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("EntityController (scopes/entity.cpp): getEntityList()") {

		SUBCASE("No args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "list", request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getEntityList(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Empty args (no game id)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "list", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getEntityList(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, invalid game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getEntityList(0);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, invalid game id") {

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

			response = getEntityList(gameId + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Stopped game, valid game id") {

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

			response = stopGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntityList(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("entities"));
			CHECK(response["entities"].IsArray());
			CHECK(response["entities"].Size() > 0);

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Started game, valid game id") {

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

			response = startGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntityList(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("entities"));
			CHECK(response["entities"].IsArray());
			CHECK(response["entities"].Size() > 0);

			destroyGameXML();
			destroyConfig();
		}
	}

	// For now, I'm only testing the default output driver. Later, I'll want
	// to alter the config to test the redis controller, which would return an
	// operation not supported error that I'd need to also validate.
	TEST_CASE("EntityController (scopes/entity.cpp): getOutput()") {

		SUBCASE("No args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_CHANNEL).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Empty args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_CHANNEL).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("name", "trogdor", request.GetAllocator());
			args.AddMember("channel", "display", request.GetAllocator());

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing entity name") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("channel", "display", request.GetAllocator());

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_ENTITY_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing channel") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", "trogdor", request.GetAllocator());

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_CHANNEL).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid entity name (not a string)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", 0, request.GetAllocator()); // Should be a string for a valid request
			args.AddMember("channel", "display", request.GetAllocator());

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->getOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::INVALID_ENTITY_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, Invalid channel (any non-string type)") {

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

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", gameId, request.GetAllocator());
			args.AddMember("name", "start", request.GetAllocator());
			args.AddMember("channel", 0, request.GetAllocator()); // Non-string type should make request invalid

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = EntityController::get()->getOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::INVALID_CHANNEL).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, invalid game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getOutput(0, "thisIsAnEntityName", "display");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, invalid game id") {

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

			response = getOutput(gameId + 1, "thisIsAnEntityName", "display");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, undefined entity name") {

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

			response = getOutput(gameId, "NotAnEntity", "display");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::ENTITY_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Stopped game, valid game id, entity name, and channel") {

			const char *playerName = "player";
			const char *channel = "display";

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

			response = stopGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Right now, players are the only ones who receive output
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getOutput(gameId, playerName, channel);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("messages"));
			CHECK(response["messages"].IsArray());
			CHECK(response["messages"].IsArray());
			CHECK(response["messages"].Size() > 0);

			CHECK(response["messages"][0].HasMember("timestamp"));
			CHECK(response["messages"][0]["timestamp"].IsUint());

			CHECK(response["messages"][0].HasMember("content"));
			CHECK(response["messages"][0]["content"].IsString());

			CHECK(response["messages"][0].HasMember("order"));
			CHECK(response["messages"][0]["order"].IsUint());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Started game, valid game id, entity name, and channel") {

			const char *playerName = "player";
			const char *channel = "display";

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

			response = startGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Right now, players are the only ones who receive output
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getOutput(gameId, playerName, channel);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("messages"));
			CHECK(response["messages"].IsArray());
			CHECK(response["messages"].IsArray());
			CHECK(response["messages"].Size() > 0);

			CHECK(response["messages"][0].HasMember("timestamp"));
			CHECK(response["messages"][0]["timestamp"].IsUint());

			CHECK(response["messages"][0].HasMember("content"));
			CHECK(response["messages"][0]["content"].IsString());

			CHECK(response["messages"][0].HasMember("order"));
			CHECK(response["messages"][0]["order"].IsUint());

			destroyGameXML();
			destroyConfig();
		}

		#ifdef ENABLE_REDIS

			SUBCASE("Valid game id, entity name, and channel, but Redis output driver") {

				const char *playerName = "player";
				const char *channel = "display";

				GameContainer::get()->reset();

				initGameXML();
				initConfig(true);

				rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

				CHECK(trogdor::isAscii(JSON::serialize(response)));

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				CHECK(response.HasMember("id"));
				CHECK(response["id"].IsUint());

				size_t gameId = response["id"].GetUint();

				response = stopGame(gameId);

				CHECK(trogdor::isAscii(JSON::serialize(response)));

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				// Right now, players are the only ones who receive output
				response = createPlayer(gameId, playerName);

				CHECK(trogdor::isAscii(JSON::serialize(response)));

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				response = getOutput(gameId, playerName, channel);

				CHECK(trogdor::isAscii(JSON::serialize(response)));

				// getOutput() is unsupported when we're using the Redis
				// output driver.
				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

				destroyGameXML();
				destroyConfig();
			}

		#endif
	}

	TEST_CASE("EntityController (scopes/entity.cpp): appendOutput()") {

		SUBCASE("No args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());

			rapidjson::Document response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_OUTPUT_MESSAGE).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Empty args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_OUTPUT_MESSAGE).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing game id") {

			const char *entityName = "player";
			const char *message = "test";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("name", rapidjson::StringRef(entityName), request.GetAllocator());
			args.AddMember("message", rapidjson::StringRef(message), request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing entity name") {

			const char *message = "test";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("message", rapidjson::StringRef(message), request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::MISSING_ENTITY_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, invalid game id") {

			const char *message = "test";
			const char *entityName = "player";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = appendOutput(0, entityName, message);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, invalid game id") {

			const char *message = "test";
			const char *entityName = "player";

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

			response = appendOutput(gameId + 1, entityName, message);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, invalid entity name") {

			const char *message = "test";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", 0, request.GetAllocator()); // Valid entity name should be a string
			args.AddMember("message", rapidjson::StringRef(message), request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::INVALID_ENTITY_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, invalid entity name") {

			const char *message = "test";

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

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", gameId, request.GetAllocator());
			args.AddMember("name", 0, request.GetAllocator()); // Valid entity name should be a string
			args.AddMember("message", rapidjson::StringRef(message), request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::INVALID_ENTITY_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid channel (any non-string type)") {

			const char *entityName = "player";
			const char *message = "test";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", rapidjson::StringRef(entityName), request.GetAllocator());
			args.AddMember("message", rapidjson::StringRef(message), request.GetAllocator());
			args.AddMember("channel", 0, request.GetAllocator()); // Channel should be a string

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::INVALID_CHANNEL).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid message (any non-string type)") {

			const char *entityName = "player";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);
			rapidjson::Value invalidMessage(rapidjson::kArrayType);

			// Valid message can be any scalar but can't be an array or object
			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", rapidjson::StringRef(entityName), request.GetAllocator());
			args.AddMember("message", invalidMessage, request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "entity", request.GetAllocator());
			request.AddMember("action", "output", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = EntityController::get()->appendOutput(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(EntityController::INVALID_OUTPUT_MESSAGE).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Stopped game, valid game id, entity name, channel, and message") {

			// By using a channel not used by the core library, I can guarantee
			// I'll only reflect back what I send via appendOutput().
			const char *playerName = "player";
			const char *channel = "test";
			const char *message = "I'm a message!";

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

			response = stopGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Right now, players are the only ones who receive output
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// First, append the message
			response = appendOutput(gameId, playerName, message, channel);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Next, verify that we can read that message back
			response = getOutput(gameId, playerName, channel);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("messages"));
			CHECK(response["messages"].IsArray());
			CHECK(1 == response["messages"].Size());

			CHECK(response["messages"][0].HasMember("timestamp"));
			CHECK(response["messages"][0]["timestamp"].IsUint());
			CHECK(response["messages"][0]["timestamp"].GetUint() > 0);

			CHECK(response["messages"][0].HasMember("order"));
			CHECK(response["messages"][0]["order"].IsUint());

			std::string expectedMessageOutput = std::string(message) + '\n';
			CHECK(response["messages"][0].HasMember("content"));
			CHECK(response["messages"][0]["content"].IsString());
			CHECK(0 == expectedMessageOutput.compare(response["messages"][0]["content"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Started game, valid game id, entity name, channel, and message") {

			// By using a channel not used by the core library, I can guarantee
			// I'll only reflect back what I send via appendOutput().
			const char *playerName = "player";
			const char *channel = "test";
			const char *message = "I'm a message!";

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

			response = startGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Right now, players are the only ones who receive output
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// First, append the message
			response = appendOutput(gameId, playerName, message, channel);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Next, verify that we can read that message back
			response = getOutput(gameId, playerName, channel);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("messages"));
			CHECK(response["messages"].IsArray());
			CHECK(1 == response["messages"].Size());

			CHECK(response["messages"][0].HasMember("timestamp"));
			CHECK(response["messages"][0]["timestamp"].IsUint());
			CHECK(response["messages"][0]["timestamp"].GetUint() > 0);

			CHECK(response["messages"][0].HasMember("order"));
			CHECK(response["messages"][0]["order"].IsUint());

			std::string expectedMessageOutput = std::string(message) + '\n';
			CHECK(response["messages"][0].HasMember("content"));
			CHECK(response["messages"][0]["content"].IsString());
			CHECK(0 == expectedMessageOutput.compare(response["messages"][0]["content"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Stopped game, default channel, valid game id, entity name, and message") {

			const char *playerName = "player";
			const char *message = "I'm a message!";

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

			response = stopGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Right now, players are the only ones who receive output
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// First, append the message to the default output channel
			response = appendOutput(gameId, playerName, message);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Next, verify that we can read that message back
			response = getOutput(gameId, playerName, Entity::DEFAULT_OUTPUT_CHANNEL);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("messages"));
			CHECK(response["messages"].IsArray());
			CHECK(response["messages"].Size() > 0);

			// Since there are going to be other output messages too, I need
			// to loop through each and verify that one of them contains the
			// appended message.
			bool foundMsg = false;

			for (auto i = response["messages"].Begin(); i != response["messages"].End(); i++) {

				CHECK(i->HasMember("timestamp"));
				CHECK((*i)["timestamp"].IsUint());
				CHECK((*i)["timestamp"].GetUint() > 0);

				CHECK(i->HasMember("order"));
				CHECK((*i)["order"].IsUint());

				std::string expectedMessageOutput = std::string(message) + '\n';

				CHECK(i->HasMember("content"));
				CHECK((*i)["content"].IsString());

				if (0 == expectedMessageOutput.compare((*i)["content"].GetString())) {
					foundMsg = true;
				}
			}

			CHECK(foundMsg);

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Started game, default channel, valid game id, entity name, and message") {

			const char *playerName = "player";
			const char *message = "I'm a message!";

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

			response = startGame(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Right now, players are the only ones who receive output
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// First, append the message to the default output channel
			response = appendOutput(gameId, playerName, message);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Next, verify that we can read that message back
			response = getOutput(gameId, playerName, Entity::DEFAULT_OUTPUT_CHANNEL);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("messages"));
			CHECK(response["messages"].IsArray());
			CHECK(response["messages"].Size() > 0);

			// Since there are going to be other output messages too, I need
			// to loop through each and verify that one of them contains the
			// appended message.
			bool foundMsg = false;

			for (auto i = response["messages"].Begin(); i != response["messages"].End(); i++) {

				CHECK(i->HasMember("timestamp"));
				CHECK((*i)["timestamp"].IsUint());
				CHECK((*i)["timestamp"].GetUint() > 0);

				CHECK(i->HasMember("order"));
				CHECK((*i)["order"].IsUint());

				std::string expectedMessageOutput = std::string(message) + '\n';

				CHECK(i->HasMember("content"));
				CHECK((*i)["content"].IsString());

				if (0 == expectedMessageOutput.compare((*i)["content"].GetString())) {
					foundMsg = true;
				}
			}

			CHECK(foundMsg);

			destroyGameXML();
			destroyConfig();
		}
	}
}
