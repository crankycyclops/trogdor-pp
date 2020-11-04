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

		CHECK(trogdor::isAscii(JSON::serialize(response)));

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

		CHECK(trogdor::isAscii(JSON::serialize(response)));

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

	TEST_CASE("PlayerController (scopes/player.cpp): createPlayer()") {

		SUBCASE("No arguments") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->createPlayer(request);

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

		SUBCASE("Empty arguments") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->createPlayer(request);

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

		SUBCASE("Missing required game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", "player", request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->createPlayer(request);

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

		SUBCASE("Missing required player name") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->createPlayer(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(PlayerController::MISSING_PLAYER_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid player name (not a string type)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", 0, request.GetAllocator()); // only a string type is valid

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->createPlayer(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(PlayerController::INVALID_PLAYER_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid player name (doesn't match valid regex)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", "player!@#$%^&*()`~", request.GetAllocator()); // name contains illegal characters

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->createPlayer(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(PlayerController::INVALID_PLAYER_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid request, non-existent game id") {

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

			response = createPlayer(gameId + 1, "test");

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

		SUBCASE("Valid request, stopped game") {

			const char *playerName = "player";

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

			// Verify that player creation was successful
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("player"));
			CHECK(response["player"].IsObject());

			CHECK(response["player"].HasMember("name"));
			CHECK(response["player"]["name"].IsString());
			CHECK(0 == std::string(playerName).compare(response["player"]["name"].GetString()));

			CHECK(response["player"].HasMember("type"));
			CHECK(response["player"]["type"].IsString());
			CHECK(0 == std::string("player").compare(response["player"]["type"].GetString()));

			// Verify that we can echo back the newly created user
			response = getEntity(gameId, playerName, "player");

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("entity"));
			CHECK(response["entity"].IsObject());

			CHECK(response["entity"].HasMember("name"));
			CHECK(response["entity"]["name"].IsString());
			CHECK(0 == std::string(playerName).compare(response["entity"]["name"].GetString()));

			CHECK(response["entity"].HasMember("type"));
			CHECK(response["entity"]["type"].IsString());
			CHECK(0 == std::string("player").compare(response["entity"]["type"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid request, started game") {

			const char *playerName = "player";

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

			// Verify that player creation was successful
			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("player"));
			CHECK(response["player"].IsObject());

			CHECK(response["player"].HasMember("name"));
			CHECK(response["player"]["name"].IsString());
			CHECK(0 == std::string(playerName).compare(response["player"]["name"].GetString()));

			CHECK(response["player"].HasMember("type"));
			CHECK(response["player"]["type"].IsString());
			CHECK(0 == std::string("player").compare(response["player"]["type"].GetString()));

			// Verify that we can echo back the newly created user
			response = getEntity(gameId, playerName, "player");

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("entity"));
			CHECK(response["entity"].IsObject());

			CHECK(response["entity"].HasMember("name"));
			CHECK(response["entity"]["name"].IsString());
			CHECK(0 == std::string(playerName).compare(response["entity"]["name"].GetString()));

			CHECK(response["entity"].HasMember("type"));
			CHECK(response["entity"]["type"].IsString());
			CHECK(0 == std::string("player").compare(response["entity"]["type"].GetString()));

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("PlayerController (scopes/player.cpp): destroyPlayer()") {

		SUBCASE("No arguments") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->destroyPlayer(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Empty arguments") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->destroyPlayer(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing required game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", "player", request.GetAllocator());

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->destroyPlayer(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing required player name") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->destroyPlayer(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(PlayerController::MISSING_PLAYER_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid player name (not a string type)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", 0, request.GetAllocator()); // only a string type is valid

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->destroyPlayer(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(PlayerController::INVALID_PLAYER_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid request, non-existent game id") {

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

			response = destroyPlayer(gameId + 1, "test");

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

		SUBCASE("Valid request, valid game id, non-existent player name") {

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

			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = destroyPlayer(gameId, "NotAPlayer");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(PlayerController::PLAYER_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid request, stopped game, no message") {

			const char *playerName = "player";

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

			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = destroyPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntity(gameId, playerName, "player");

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

		SUBCASE("Valid request, started game, no message") {

			const char *playerName = "player";

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

			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = destroyPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntity(gameId, playerName, "player");

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

		// Right now, there's not really a good way to check if the message
		// was appended to the player's output stream. Instead, we just check
		// to ensure that the request with the extra parameter is successful.
		SUBCASE("Valid request, stopped game, with message") {

			const char *playerName = "player";
			const char *message = "You are terminated.";

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

			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = destroyPlayer(gameId, playerName, message);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntity(gameId, playerName, "player");

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

		// Right now, there's not really a good way to check if the message
		// was appended to the player's output stream. Instead, we just check
		// to ensure that the request with the extra parameter is successful.
		SUBCASE("Valid request, started game, with message") {

			const char *playerName = "player";
			const char *message = "You are terminated.";

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

			response = createPlayer(gameId, playerName);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = destroyPlayer(gameId, playerName, message);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getEntity(gameId, playerName, "player");

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
	}

	TEST_CASE("PlayerController (scopes/player.cpp): postInput()") {

		SUBCASE("No arguments") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("action", "input", request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->postInput(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Empty arguments") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("action", "input", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->postInput(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing required game id") {
			// TODO
		}

		SUBCASE("Missing required player name") {
			// TODO
		}

		SUBCASE("Invalid player name (not a string type)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("game_id", 0, request.GetAllocator());
			args.AddMember("name", 0, request.GetAllocator()); // only a string type is valid

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "player", request.GetAllocator());
			request.AddMember("action", "input", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = PlayerController::get()->postInput(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(PlayerController::INVALID_PLAYER_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing required command") {
			// TODO
		}

		SUBCASE("Invalid command (non-string type)") {
			// TODO
		}

		SUBCASE("Valid request, non-existent game id") {
			// TODO
		}

		SUBCASE("Valid request, existing game id, non-existent player name") {
			// TODO
		}

		SUBCASE("Valid request, stopped game") {
			// TODO
		}

		SUBCASE("Valid request, started game") {
			// TODO
		}
	}
}
