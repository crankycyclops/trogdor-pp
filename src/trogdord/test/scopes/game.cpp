#include <doctest.h>
#include "helper.h"


// Test game names
static const char *gameName = "myGame";
static const char *gameName2 = "yourGame";

TEST_SUITE("GameController (scopes/game.cpp)") {

	TEST_CASE("GameController (scopes/game.cpp): getDefinitionList()") {

		initGameXML();
		initConfig();

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", "get", request.GetAllocator());
		request.AddMember("scope", "global", request.GetAllocator());
		request.AddMember("action", "statistics", request.GetAllocator());

		rapidjson::Document response = GameController::get()->getDefinitionList(request);

		CHECK(trogdor::isAscii(JSON::serialize(response)));

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("definitions"));
		CHECK(response["definitions"].IsArray());
		CHECK(1 == response["definitions"].Size());
		CHECK(response["definitions"][0].IsString());
		CHECK(0 == gameXMLRelativeFilename.compare(response["definitions"][0].GetString()));

		destroyGameXML();
		destroyConfig();
	}

	TEST_CASE("GameController (scopes/game.cpp): createGame()") {

		SUBCASE("Missing required game name") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember(
				"definition",
				rapidjson::StringRef(gameXMLRelativeFilename.c_str()),
				request.GetAllocator()
			);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::MISSING_REQUIRED_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game name is not a string") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", false, request.GetAllocator());
			args.AddMember(
				"definition",
				rapidjson::StringRef(gameXMLRelativeFilename.c_str()),
				request.GetAllocator()
			);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_NAME).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Missing required definition") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", "test", request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::MISSING_REQUIRED_DEFINITION).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Definition is not a string") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", "test", request.GetAllocator());
			args.AddMember("definition", false, request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_DEFINITION).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Definition is an absolute path") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			std::string definition;
			definition += STD_FILESYSTEM::path::preferred_separator;
			definition += gameXMLRelativeFilename;

			args.AddMember("name", "test", request.GetAllocator());
			args.AddMember("definition", rapidjson::StringRef(definition.c_str()), request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::DEFINITION_NOT_RELATIVE).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Optional meta argument passed as incorrect type (array)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);
			rapidjson::Value metavalue(rapidjson::kArrayType);

			args.AddMember("name", "test", request.GetAllocator());
			args.AddMember("definition", rapidjson::StringRef(gameXMLRelativeFilename.c_str()), request.GetAllocator());
			args.AddMember("metakey", metavalue, request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_META).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Definition doesn't exist") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", "test", request.GetAllocator());
			args.AddMember("definition", "doesntexist.xml", request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INTERNAL_ERROR == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());

			// This last check helps verify that the string wasn't corrupted
			// (I run into this with RapidJSON a lot, especially when I try
			// to insert std::string values into JSON objects.)
			CHECK(trogdor::isAscii(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation is successful without meta") {

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
			size_t id = response["id"].GetUint();

			response = getGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("name"));
			CHECK(response["name"].IsString());
			CHECK(0 == std::string(gameName).compare(response["name"].GetString()));

			CHECK(response.HasMember("definition"));
			CHECK(response["definition"].IsString());
			CHECK(0 == std::string(gameXMLRelativeFilename).compare(response["definition"].GetString()));

			rapidjson::Document getMetaRequest(rapidjson::kObjectType);
			rapidjson::Value getMetaArgs(rapidjson::kObjectType);

			getMetaArgs.AddMember("id", id, getMetaRequest.GetAllocator());

			getMetaRequest.AddMember("method", "get", getMetaRequest.GetAllocator());
			getMetaRequest.AddMember("scope", "game", getMetaRequest.GetAllocator());
			getMetaRequest.AddMember("action", "meta", getMetaRequest.GetAllocator());
			getMetaRequest.AddMember("args", getMetaArgs, getMetaRequest.GetAllocator());

			response = GameController::get()->getMeta(getMetaRequest);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("meta"));
			CHECK(response["meta"].IsObject());
			CHECK(response["meta"].MemberBegin() == response["meta"].MemberEnd());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation is successful with valid meta") {

			const char *argTitle = "My Title";
			const size_t argPositiveNumber = 1;
			const int argNegativeNumber = -1;
			const bool argBool = false;

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", rapidjson::StringRef(gameName), request.GetAllocator());
			args.AddMember("title", rapidjson::StringRef(argTitle), request.GetAllocator());
			args.AddMember("positive", argPositiveNumber, request.GetAllocator());
			args.AddMember("negative", argNegativeNumber, request.GetAllocator());
			args.AddMember("boolean", argBool, request.GetAllocator());
			args.AddMember(
				"definition",
				rapidjson::StringRef(gameXMLRelativeFilename.c_str()),
				request.GetAllocator()
			);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));

			CHECK(response["id"].IsUint());
			size_t id = response["id"].GetUint();

			response = getGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("name"));
			CHECK(response["name"].IsString());
			CHECK(0 == std::string(gameName).compare(response["name"].GetString()));

			CHECK(response.HasMember("definition"));
			CHECK(response["definition"].IsString());
			CHECK(0 == std::string(gameXMLRelativeFilename).compare(response["definition"].GetString()));

			rapidjson::Document getMetaRequest(rapidjson::kObjectType);
			rapidjson::Value getMetaArgs(rapidjson::kObjectType);

			getMetaArgs.AddMember("id", id, getMetaRequest.GetAllocator());

			getMetaRequest.AddMember("method", "get", getMetaRequest.GetAllocator());
			getMetaRequest.AddMember("scope", "game", getMetaRequest.GetAllocator());
			getMetaRequest.AddMember("action", "meta", getMetaRequest.GetAllocator());
			getMetaRequest.AddMember("args", getMetaArgs, getMetaRequest.GetAllocator());

			response = GameController::get()->getMeta(getMetaRequest);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("meta"));
			CHECK(response["meta"].IsObject());

			CHECK(response["meta"].HasMember("title"));
			CHECK(response["meta"]["title"].IsString());
			CHECK(0 == std::string(argTitle).compare(response["meta"]["title"].GetString()));

			CHECK(response["meta"].HasMember("positive"));
			CHECK(response["meta"]["positive"].IsString());
			CHECK(0 == std::to_string(argPositiveNumber).compare(response["meta"]["positive"].GetString()));

			CHECK(response["meta"].HasMember("negative"));
			CHECK(response["meta"]["negative"].IsString());
			CHECK(0 == std::to_string(argNegativeNumber).compare(response["meta"]["negative"].GetString()));

			CHECK(response["meta"].HasMember("boolean"));
			CHECK(response["meta"]["boolean"].IsString());
			CHECK(0 == std::string(argBool ? "true" : "false").compare(response["meta"]["boolean"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation fails because of invalid meta (object)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);
			rapidjson::Value metaValue(rapidjson::kObjectType);

			args.AddMember("name", rapidjson::StringRef(gameName), request.GetAllocator());
			args.AddMember("metavalue", metaValue, request.GetAllocator());
			args.AddMember(
				"definition",
				rapidjson::StringRef(gameXMLRelativeFilename.c_str()),
				request.GetAllocator()
			);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_META).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation fails because of invalid meta (array)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);
			rapidjson::Value metaValue(rapidjson::kArrayType);

			args.AddMember("name", rapidjson::StringRef(gameName), request.GetAllocator());
			args.AddMember("metavalue", metaValue, request.GetAllocator());
			args.AddMember(
				"definition",
				rapidjson::StringRef(gameXMLRelativeFilename.c_str()),
				request.GetAllocator()
			);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_META).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): destroyGame()") {

		SUBCASE("Game ID is missing") {

			GameContainer::get()->reset();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());

			rapidjson::Document response = GameController::get()->destroyGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());

			// This last check helps verify that the string wasn't corrupted
			// (I run into this with RapidJSON a lot, especially when I try
			// to insert std::string values into JSON objects.)
			CHECK(trogdor::isAscii(response["message"].GetString()));
		}

		SUBCASE("Game ID is missing with empty args value") {

			GameContainer::get()->reset();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->destroyGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());

			// This last check helps verify that the string wasn't corrupted
			// (I run into this with RapidJSON a lot, especially when I try
			// to insert std::string values into JSON objects.)
			CHECK(trogdor::isAscii(response["message"].GetString()));
		}

		SUBCASE("Passing non-existent game ID") {

			GameContainer::get()->reset();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("id", 1, request.GetAllocator());

			request.AddMember("method", "delete", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->destroyGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));
		}

		SUBCASE("Successful destruction of game") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			// Step 1: create a game and store the ID
			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));

			CHECK(response["id"].IsUint());
			size_t id = response["id"].GetUint();

			// Make sure getGame() can find it
			response = getGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Delete the game
			response = destroyGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Make sure getGame() can no longer find it
			response = getGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			// Make sure destroyGame() returns 404 if we call it again with the same id
			response = destroyGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getGame()") {

		SUBCASE("Without args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());

			rapidjson::Document response = GameController::get()->getGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());
			CHECK( 0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("With args but missing game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());

			rapidjson::Document response = GameController::get()->getGame(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());
			CHECK( 0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			// Step 1: test with no games running
			rapidjson::Document response = getGame(1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());
			CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			// Step 2: test with a game running and an invalid id
			response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			response = getGame(id + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id") {

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

			size_t id = response["id"].GetUint();

			response = getGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());
			CHECK(id == response["id"].GetUint());

			CHECK(response.HasMember("name"));
			CHECK(response["name"].IsString());
			CHECK(0 == std::string(gameName).compare(response["name"].GetString()));

			CHECK(response.HasMember("definition"));
			CHECK(response["definition"].IsString());
			CHECK(0 == std::string(gameXMLRelativeFilename).compare(response["definition"].GetString()));

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(false == response["is_running"].GetBool());

			response = destroyGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getGame(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getMeta()") {

		SUBCASE("Without args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document metaRequest(rapidjson::kObjectType);

			metaRequest.AddMember("method", "get", metaRequest.GetAllocator());
			metaRequest.AddMember("scope", "game", metaRequest.GetAllocator());
			metaRequest.AddMember("action", "meta", metaRequest.GetAllocator());

			rapidjson::Document response = GameController::get()->getMeta(metaRequest);

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

		SUBCASE("With empty args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document metaRequest(rapidjson::kObjectType);
			rapidjson::Document metaArgs(rapidjson::kObjectType);

			metaRequest.AddMember("method", "get", metaRequest.GetAllocator());
			metaRequest.AddMember("scope", "game", metaRequest.GetAllocator());
			metaRequest.AddMember("action", "meta", metaRequest.GetAllocator());
			metaRequest.AddMember("args", metaArgs, metaRequest.GetAllocator());

			rapidjson::Document response = GameController::get()->getMeta(metaRequest);

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

		SUBCASE("Invalid game id, meta arg is not an array") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(
				gameName,
				gameXMLRelativeFilename.c_str(),
				testMeta
			);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			rapidjson::Document metaRequest(rapidjson::kObjectType);
			rapidjson::Value metaRequestArgs(rapidjson::kObjectType);

			metaRequestArgs.AddMember("id", id + 1, metaRequest.GetAllocator());
			metaRequestArgs.AddMember("meta", rapidjson::Value(rapidjson::kObjectType), metaRequest.GetAllocator());

			metaRequest.AddMember("method", "get", metaRequest.GetAllocator());
			metaRequest.AddMember("scope", "game", metaRequest.GetAllocator());
			metaRequest.AddMember("args", metaRequestArgs, metaRequest.GetAllocator());

			response = GameController::get()->getMeta(metaRequest);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id, meta arg is not an array") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(
				gameName,
				gameXMLRelativeFilename.c_str(),
				testMeta
			);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			rapidjson::Document metaRequest(rapidjson::kObjectType);
			rapidjson::Value metaRequestArgs(rapidjson::kObjectType);

			metaRequestArgs.AddMember("id", id, metaRequest.GetAllocator());
			metaRequestArgs.AddMember("meta", rapidjson::Value(rapidjson::kObjectType), metaRequest.GetAllocator());

			metaRequest.AddMember("method", "get", metaRequest.GetAllocator());
			metaRequest.AddMember("scope", "game", metaRequest.GetAllocator());
			metaRequest.AddMember("args", metaRequestArgs, metaRequest.GetAllocator());

			response = GameController::get()->getMeta(metaRequest);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_META_KEYS).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id, no meta argument") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(
				gameName,
				gameXMLRelativeFilename.c_str(),
				testMeta
			);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			// id + 1 is guaranteed not to exist
			response = getMeta(id + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id, no meta argument") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(
				gameName,
				gameXMLRelativeFilename.c_str(),
				testMeta
			);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			response = getMeta(id);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("meta"));
			CHECK(response["meta"].IsObject());

			// rapidjson::Value::Size() only works for arrays, not objects
			size_t metaCount = 0;
			for (auto i = response["meta"].MemberBegin(); i != response["meta"].MemberEnd(); i++) {

				CHECK(i->name.IsString());
				CHECK(i->value.IsString());
				CHECK(0 == testMeta[i->name.GetString()].compare(i->value.GetString()));

				metaCount++;
			}

			CHECK(metaCount == testMeta.size());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id, with meta argument") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(
				gameName,
				gameXMLRelativeFilename.c_str(),
				testMeta
			);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			// id + 1 is guaranteed not to exist
			response = getMeta(id + 1, {"key1"});

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id, with meta argument") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(
				gameName,
				gameXMLRelativeFilename.c_str(),
				testMeta
			);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			// Test valid value
			response = getMeta(id, {"key1"});

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("meta"));
			CHECK(response["meta"].IsObject());

			// rapidjson::Value::Size() only works for arrays, not objects
			size_t metaCount = 0;
			for (auto i = response["meta"].MemberBegin(); i != response["meta"].MemberEnd(); i++) {

				CHECK(i->name.IsString());
				CHECK(i->value.IsString());
				CHECK(0 == testMeta[i->name.GetString()].compare(i->value.GetString()));

				metaCount++;
			}

			CHECK(1 == metaCount);

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): setMeta()") {

		SUBCASE("Without args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());

			rapidjson::Document response = GameController::get()->setMeta(request);

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

		SUBCASE("With empty args") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->setMeta(request);

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

		SUBCASE("Invalid id argument, no meta argument") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			args.AddMember("id", 1, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid id argument, no meta argument") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);

			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			args.AddMember("id", response["id"].GetUint(), request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = GameController::get()->setMeta(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::MISSING_META).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id, no games running") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);
			rapidjson::Document meta(rapidjson::kObjectType);

			meta.AddMember("key1", "value1", request.GetAllocator());

			args.AddMember("id", 1, request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id, one game running") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);
			rapidjson::Document meta(rapidjson::kObjectType);

			meta.AddMember("key1", "value1", request.GetAllocator());

			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			args.AddMember("id", response["id"].GetUint() + 1, request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id, invalid meta (array)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);
			rapidjson::Document meta(rapidjson::kArrayType);

			args.AddMember("id", 1, request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id, invalid meta (array)") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);
			rapidjson::Document meta(rapidjson::kArrayType);

			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			args.AddMember("id", response["id"].GetUint(), request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_META).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id, empty meta") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);
			rapidjson::Document meta(rapidjson::kObjectType);

			args.AddMember("id", 1, request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id, empty meta") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Document args(rapidjson::kObjectType);
			rapidjson::Document meta(rapidjson::kObjectType);

			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			args.AddMember("id", response["id"].GetUint(), request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::MISSING_META).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id, one meta value") {

			const char *key1 = "key1";
			const char *value1 = "value1";

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
			rapidjson::Document meta(rapidjson::kObjectType);

			meta.AddMember(rapidjson::StringRef(key1), rapidjson::StringRef(value1), request.GetAllocator());

			args.AddMember("id", response["id"].GetUint(), request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getMeta(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("meta"));
			CHECK(response["meta"].IsObject());

			// Because rapidjson::Value.Size() isn't defined for objects. Grr.
			size_t count = 0;

			for (auto i = response["meta"].MemberBegin(); i != response["meta"].MemberEnd(); i++) {
				count++;
			}

			CHECK(1 == count);

			CHECK(response["meta"].HasMember(key1));
			CHECK(response["meta"][key1].IsString());
			CHECK(0 == std::string(value1).compare(response["meta"][key1].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Valid game id, two meta values") {

			const char *key1 = "key1";
			const char *value1 = "value1";

			const char *key2 = "key2";
			const char *value2 = "value2";

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
			rapidjson::Document meta(rapidjson::kObjectType);

			meta.AddMember(rapidjson::StringRef(key1), rapidjson::StringRef(value1), request.GetAllocator());
			meta.AddMember(rapidjson::StringRef(key2), rapidjson::StringRef(value2), request.GetAllocator());

			args.AddMember("id", response["id"].GetUint(), request.GetAllocator());
			args.AddMember("meta", meta, request.GetAllocator());

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "meta", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			response = GameController::get()->setMeta(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getMeta(gameId);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("meta"));
			CHECK(response["meta"].IsObject());

			// Because rapidjson::Value.Size() isn't defined for objects. Grr.
			size_t count = 0;

			for (auto i = response["meta"].MemberBegin(); i != response["meta"].MemberEnd(); i++) {
				count++;
			}

			CHECK(2 == count);

			CHECK(response["meta"].HasMember(key1));
			CHECK(response["meta"][key1].IsString());
			CHECK(0 == std::string(value1).compare(response["meta"][key1].GetString()));

			CHECK(response["meta"].HasMember(key2));
			CHECK(response["meta"][key2].IsString());
			CHECK(0 == std::string(value2).compare(response["meta"][key2].GetString()));

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getDefinitionList()") {

		SUBCASE("definitions directory doesn't exist (results in internal error)") {

			GameContainer::get()->reset();
			initConfig();

			// initGameXML() creates the subdirectory as well as creates the
			// definition file, so by not calling this and not manually
			// creating that directory, this request should fail with a
			// status of Response::STATUS_INTERNAL_ERROR.
			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "definitions", request.GetAllocator());

			rapidjson::Document response = GameController::get()->getDefinitionList(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INTERNAL_ERROR == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Response::INTERNAL_ERROR_MSG).compare(response["message"].GetString()));

			destroyConfig();
		}

		SUBCASE("No definitions available") {

			GameContainer::get()->reset();
			initConfig();

			// Create the directory for definitions like we do in initGameXML(),
			// but don't create the XML file
			gameXMLLocation = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "trogtest";

			STD_FILESYSTEM::create_directory(gameXMLLocation);

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "definitions", request.GetAllocator());

			rapidjson::Document response = GameController::get()->getDefinitionList(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("definitions"));
			CHECK(response["definitions"].IsArray());
			CHECK(0 == response["definitions"].Size());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One definition available") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "definitions", request.GetAllocator());

			rapidjson::Document response = GameController::get()->getDefinitionList(request);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("definitions"));
			CHECK(response["definitions"].IsArray());
			CHECK(1 == response["definitions"].Size());
			CHECK(response["definitions"][0].IsString());
			CHECK(0 == std::string(gameXMLRelativeFilename).compare(response["definitions"][0].GetString()));

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getGameList()") {

		SUBCASE("No games running, without meta, without filters") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getGameList();

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game running, without meta, without filters") {

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

			size_t id = response["id"].GetUint();

			response = getGameList();

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(1 == response["games"].Size());

			CHECK(response["games"][0].HasMember("id"));
			CHECK(response["games"][0]["id"].IsUint());
			CHECK(id == response["games"][0]["id"].GetUint());

			CHECK(response["games"][0].HasMember("name"));
			CHECK(response["games"][0]["name"].IsString());
			CHECK(trogdor::isAscii(response["games"][0]["name"].GetString()));
			CHECK(0 == std::string(gameName).compare(response["games"][0]["name"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games running, with meta, without filters") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getGameList({"key1", "key2"});

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game running, with meta, without filters") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(
				gameName,
				gameXMLRelativeFilename.c_str(),
				{{"key1", "value1"}, {"key2", "value2"}}
			);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			response = getGameList({"key1", "key2"});

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(1 == response["games"].Size());
			CHECK(response["games"][0].IsObject());

			CHECK(1 == response["games"][0].HasMember("id"));
			CHECK(response["games"][0]["id"].IsUint());
			CHECK(id == response["games"][0]["id"].GetUint());

			CHECK(1 == response["games"][0].HasMember("name"));
			CHECK(response["games"][0]["name"].IsString());
			CHECK(0 == std::string(gameName).compare(response["games"][0]["name"].GetString()));

			CHECK(1 == response["games"][0].HasMember("key1"));
			CHECK(response["games"][0]["key1"].IsString());
			CHECK(0 == std::string("value1").compare(response["games"][0]["key1"].GetString()));

			CHECK(1 == response["games"][0].HasMember("key2"));
			CHECK(response["games"][0]["key2"].IsString());
			CHECK(0 == std::string("value2").compare(response["games"][0]["key2"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games running, without meta, with filters") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			// The empty string should translate to a null JSON value, which
			// should just act as if there are no filters.
			rapidjson::Document response = getGameList({}, "");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Empty filter group
			response = getGameList({}, "{}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Single filter
			response = getGameList({}, "{\"is_running\":true}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Single filter group, multiple filters
			response = getGameList({}, "{\"is_running\":true, \"name_starts\":\"a\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Filter union of groups
			response = getGameList({}, "[{\"is_running\":true}, {\"is_running\":false}]");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Four games running, without meta, with filters") {

			// Step 1: Setup up games

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

			size_t myGameIdStarted = response["id"].GetUint();

			response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t myGameIdStopped = response["id"].GetUint();

			response = createGame(gameName2, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t yourGameIdStarted = response["id"].GetUint();

			response = createGame(gameName2, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t yourGameIdStopped = response["id"].GetUint();

			response = startGame(myGameIdStarted);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = startGame(yourGameIdStarted);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Step 2: Begin Verifying game lists

			// The empty string should translate to a null JSON value, which
			// should just act as if there are no filters.
			response = getGameList({}, "");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(4 == response["games"].Size());

			// Empty filter group
			response = getGameList({}, "{}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(4 == response["games"].Size());

			// Single filter #1
			response = getGameList({}, "{\"is_running\":true}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool startedIdsValid = myGameIdStarted == (*i)["id"].GetUint() || yourGameIdStarted == (*i)["id"].GetUint();

				CHECK(startedIdsValid);
			}

			// Single filter #2
			response = getGameList({}, "{\"is_running\":false}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = myGameIdStopped == (*i)["id"].GetUint() || yourGameIdStopped == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);
			}

			// Single filter #3
			response = getGameList({}, "{\"name_starts\":\"my\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = myGameIdStopped == (*i)["id"].GetUint() || myGameIdStarted == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);
			}

			// Single filter #4
			response = getGameList({}, "{\"name_starts\":\"your\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = yourGameIdStopped == (*i)["id"].GetUint() || yourGameIdStarted == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);
			}

			// Filter group #1
			response = getGameList({}, "{\"is_running\":true, \"name_starts\":\"my\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(1 == response["games"].Size());

			CHECK(response["games"][0].HasMember("id"));
			CHECK(response["games"][0]["id"].IsUint());
			CHECK(myGameIdStarted == response["games"][0]["id"].GetUint());

			// Filter group #2
			response = getGameList({}, "{\"is_running\":false, \"name_starts\":\"your\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(1 == response["games"].Size());

			CHECK(response["games"][0].HasMember("id"));
			CHECK(response["games"][0]["id"].IsUint());
			CHECK(yourGameIdStopped == response["games"][0]["id"].GetUint());

			// Filter group #3 (contradiction)
			response = getGameList({}, "{\"is_running\":false, \"is_running\":true}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Filter group #4 (contradiction)
			response = getGameList({}, "{\"name_starts\":\"my\", \"name_starts\":\"your\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Filter union
			response = getGameList({}, "[{\"is_running\":true, \"name_starts\":\"your\"}, {\"is_running\":false, \"name_starts\":\"my\"}]");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = yourGameIdStarted == (*i)["id"].GetUint() || myGameIdStopped == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);
			}

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games running, with meta, with filters") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			// The empty string should translate to a null JSON value, which
			// should just act as if there are no filters.
			rapidjson::Document response = getGameList({"key1", "key2"}, "");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Empty filter group
			response = getGameList({"key1", "key2"}, "{}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Single filter
			response = getGameList({"key1", "key2"}, "{\"is_running\":true}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Single filter group, multiple filters
			response = getGameList({"key1", "key2"}, "{\"is_running\":true, \"name_starts\":\"a\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Filter union of groups
			response = getGameList({"key1", "key2"}, "[{\"is_running\":true}, {\"is_running\":false}]");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Four games running, with meta, with filters") {

			// Step 1: Setup up games. myGameIdStopped and yourGameIdStarted
			// will have one meta value set ("key1" => "value1") and the
			// others won't. In the case of the ones that don't, an empty
			// value for "key1" should be returned along with the list.

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

			size_t myGameIdStarted = response["id"].GetUint();

			response = createGame(gameName, gameXMLRelativeFilename.c_str(), {{"key1", "value1"}});

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t myGameIdStopped = response["id"].GetUint();

			response = createGame(gameName2, gameXMLRelativeFilename.c_str(), {{"key1", "value1"}});

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t yourGameIdStarted = response["id"].GetUint();

			response = createGame(gameName2, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t yourGameIdStopped = response["id"].GetUint();

			response = startGame(myGameIdStarted);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = startGame(yourGameIdStarted);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Step 2: Begin Verifying game lists

			// The empty string should translate to a null JSON value, which
			// should just act as if there are no filters.
			response = getGameList({"key1"}, "");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(4 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				CHECK(i->HasMember("key1"));
				CHECK((*i)["key1"].IsString());

				if (myGameIdStopped == (*i)["id"].GetUint() || yourGameIdStarted == (*i)["id"].GetUint()) {
					CHECK(0 == std::string("value1").compare((*i)["key1"].GetString()));
				} else {
					CHECK(0 == std::string((*i)["key1"].GetString()).length());
				}
			}

			// Empty filter group
			response = getGameList({"key1"}, "{}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(4 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				CHECK(i->HasMember("key1"));
				CHECK((*i)["key1"].IsString());

				if (myGameIdStopped == (*i)["id"].GetUint() || yourGameIdStarted == (*i)["id"].GetUint()) {
					CHECK(0 == std::string("value1").compare((*i)["key1"].GetString()));
				} else {
					CHECK(0 == std::string((*i)["key1"].GetString()).length());
				}
			}

			// Single filter #1
			response = getGameList({"key1"}, "{\"is_running\":true}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool startedIdsValid = myGameIdStarted == (*i)["id"].GetUint() || yourGameIdStarted == (*i)["id"].GetUint();

				CHECK(startedIdsValid);
				CHECK(i->HasMember("key1"));
				CHECK((*i)["key1"].IsString());

				if (yourGameIdStarted == (*i)["id"].GetUint()) {
					CHECK(0 == std::string("value1").compare((*i)["key1"].GetString()));
				} else {
					CHECK(0 == std::string((*i)["key1"].GetString()).length());
				}
			}

			// Single filter #2
			response = getGameList({"key1"}, "{\"is_running\":false}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = myGameIdStopped == (*i)["id"].GetUint() || yourGameIdStopped == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);
				CHECK(i->HasMember("key1"));
				CHECK((*i)["key1"].IsString());

				if (myGameIdStopped == (*i)["id"].GetUint()) {
					CHECK(0 == std::string("value1").compare((*i)["key1"].GetString()));
				} else {
					CHECK(0 == std::string((*i)["key1"].GetString()).length());
				}
			}

			// Single filter #3
			response = getGameList({"key1"}, "{\"name_starts\":\"my\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = myGameIdStopped == (*i)["id"].GetUint() || myGameIdStarted == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);
				CHECK(i->HasMember("key1"));
				CHECK((*i)["key1"].IsString());

				if (myGameIdStopped == (*i)["id"].GetUint()) {
					CHECK(0 == std::string("value1").compare((*i)["key1"].GetString()));
				} else {
					CHECK(0 == std::string((*i)["key1"].GetString()).length());
				}
			}

			// Single filter #4
			response = getGameList({"key1"}, "{\"name_starts\":\"your\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = yourGameIdStopped == (*i)["id"].GetUint() || yourGameIdStarted == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);
				CHECK(i->HasMember("key1"));
				CHECK((*i)["key1"].IsString());

				if (yourGameIdStarted == (*i)["id"].GetUint()) {
					CHECK(0 == std::string("value1").compare((*i)["key1"].GetString()));
				} else {
					CHECK(0 == std::string((*i)["key1"].GetString()).length());
				}
			}

			// Filter group #1
			response = getGameList({"key1"}, "{\"is_running\":true, \"name_starts\":\"my\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(1 == response["games"].Size());

			CHECK(response["games"][0].HasMember("id"));
			CHECK(response["games"][0]["id"].IsUint());
			CHECK(myGameIdStarted == response["games"][0]["id"].GetUint());

			CHECK(response["games"][0].HasMember("key1"));
			CHECK(response["games"][0]["key1"].IsString());
			CHECK(0 == std::string(response["games"][0]["key1"].GetString()).length());

			// Filter group #2
			response = getGameList({"key1"}, "{\"is_running\":false, \"name_starts\":\"your\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(1 == response["games"].Size());

			CHECK(response["games"][0].HasMember("key1"));
			CHECK(response["games"][0]["key1"].IsString());
			CHECK(0 == std::string(response["games"][0]["key1"].GetString()).length());

			// Filter group #3 (contradiction)
			response = getGameList({"key1"}, "{\"is_running\":false, \"is_running\":true}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Filter group #4 (contradiction)
			response = getGameList({"key1"}, "{\"name_starts\":\"my\", \"name_starts\":\"your\"}");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(0 == response["games"].Size());

			// Filter union
			response = getGameList({"key1"}, "[{\"is_running\":true, \"name_starts\":\"your\"}, {\"is_running\":false, \"name_starts\":\"my\"}]");

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("games"));
			CHECK(response["games"].IsArray());
			CHECK(2 == response["games"].Size());

			for (auto i = response["games"].Begin(); i != response["games"].End(); i++) {

				CHECK(i->IsObject());
				CHECK(i->HasMember("id"));
				CHECK((*i)["id"].IsUint());

				bool stoppedIdsValid = yourGameIdStarted == (*i)["id"].GetUint() || myGameIdStopped == (*i)["id"].GetUint();

				CHECK(stoppedIdsValid);

				if (yourGameIdStarted == (*i)["id"].GetUint() || myGameIdStopped == (*i)["id"].GetUint()) {
					CHECK(0 == std::string("value1").compare((*i)["key1"].GetString()));
				} else {
					CHECK(0 == std::string((*i)["key1"].GetString()).length());
				}
			}

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): startGame()") {

		SUBCASE("Missing required game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "start", request.GetAllocator());

			// Step 1: Test with no args member
			rapidjson::Document response = GameController::get()->startGame(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			// Step 2: test with empty args member
			request.AddMember("args", rapidjson::Value(rapidjson::kObjectType).Move(), request.GetAllocator());

			response = GameController::get()->startGame(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, non-existent game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = startGame(0);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, non-existent game id") {

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

			size_t id = response["id"].GetUint();

			response = startGame(id + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, valid game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			static std::chrono::milliseconds threadSleepTime(tickInterval * 2);
			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			// Verify that game is created in a stopped state by default by
			// waiting a little while and seeing that the game's timer
			// doesn't advance.
			std::this_thread::sleep_for(threadSleepTime);

			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(!response["is_running"].GetBool());

			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(0 == response["current_time"].GetUint());

			// Next, start the game, wait a little while, and verify that it's
			// reported as running and that the timer has advanced.
			response = startGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			std::this_thread::sleep_for(threadSleepTime);
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(response["is_running"].GetBool());

			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(response["current_time"].GetUint() > 0);

			size_t currentTick = response["current_time"].GetUint();

			// Now, try starting the game again and verify that we return
			// success and that the game remains running.
			response = startGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			std::this_thread::sleep_for(threadSleepTime);
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(response["is_running"].GetBool());

			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(response["current_time"].GetUint() > currentTick);

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): stopGame()") {

		SUBCASE("Missing required game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "set", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "stop", request.GetAllocator());

			// Step 1: Test with no args member
			rapidjson::Document response = GameController::get()->stopGame(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			// Step 2: test with empty args member
			request.AddMember("args", rapidjson::Value(rapidjson::kObjectType).Move(), request.GetAllocator());

			response = GameController::get()->stopGame(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, non-existent game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = stopGame(0);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, non-existent game id") {

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

			size_t id = response["id"].GetUint();

			response = stopGame(id + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, valid game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			static std::chrono::milliseconds threadSleepTime(tickInterval * 2);
			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			// Verify that game is created in a stopped state by default by
			// waiting a little while and seeing that the game's timer
			// doesn't advance.
			std::this_thread::sleep_for(threadSleepTime);

			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(!response["is_running"].GetBool());

			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(0 == response["current_time"].GetUint());

			// Next, start the game, wait a little while, and verify that it's
			// reported as running and that the timer has advanced.
			response = startGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			std::this_thread::sleep_for(threadSleepTime);
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(response["is_running"].GetBool());

			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(response["current_time"].GetUint() > 0);

			size_t currentTick = response["current_time"].GetUint();

			// Now, stop the game, wait a little while, and make sure that the
			// timer didn't advance and that the game is reported as having
			// been stopped.
			response = stopGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			std::this_thread::sleep_for(threadSleepTime);
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(!response["is_running"].GetBool());

			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Sometimes, we call stopGame() just as the timer is about to
			// advance, so we have to accept it advancing by one as a possible
			// result.
			bool validTick = currentTick == response["current_time"].GetUint() || currentTick == response["current_time"].GetUint() - 1;

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(validTick);

			// Finally, stop the game again, and verify that we get a
			// success status back and that the game remains stopped.
			response = stopGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			std::this_thread::sleep_for(threadSleepTime);
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(!response["is_running"].GetBool());

			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Sometimes, we call stopGame() just as the timer is about to
			// advance, so we have to accept it advancing by one as a possible
			// result.
			validTick = currentTick == response["current_time"].GetUint() || currentTick == response["current_time"].GetUint() - 1;

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(validTick);

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getIsRunning()") {

		SUBCASE("Missing required game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "is_running", request.GetAllocator());

			// Step 1: Test with no args member
			rapidjson::Document response = GameController::get()->getIsRunning(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			// Step 2: test with empty args member
			request.AddMember("args", rapidjson::Value(rapidjson::kObjectType).Move(), request.GetAllocator());

			response = GameController::get()->getIsRunning(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, non-existent game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = isRunning(0);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, non-existent game id") {

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

			size_t id = response["id"].GetUint();

			response = isRunning(id + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, successful query") {

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

			size_t id = response["id"].GetUint();

			// By default, game should start in a stopped state
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(false == response["is_running"].GetBool());

			response = startGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Now, the game should report as started
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(true == response["is_running"].GetBool());

			response = stopGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			// Now that we've stopped the game, it should once again report as stopped
			response = isRunning(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(false == response["is_running"].GetBool());

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getTime()") {

		SUBCASE("Missing required game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "time", request.GetAllocator());

			// Step 1: Test with no args member
			rapidjson::Document response = GameController::get()->getTime(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			// Step 2: test with empty args member
			request.AddMember("args", rapidjson::Value(rapidjson::kObjectType).Move(), request.GetAllocator());

			response = GameController::get()->getTime(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, non-existent game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getTime(0);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, non-existent game id") {

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

			size_t id = response["id"].GetUint();

			response = getTime(id + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, successful query") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			static std::chrono::milliseconds threadSleepTime(tickInterval * 2);
			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			// Verify that game is created in a stopped state by default by
			// waiting a little while and seeing that the game's timer
			// doesn't advance.
			std::this_thread::sleep_for(threadSleepTime);
			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(0 == response["current_time"].GetUint());

			// Now, start the game and wait a little while before getting the
			// time and confirm that the timer has advanced.
			response = startGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			std::this_thread::sleep_for(threadSleepTime);
			response = getTime(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(response["current_time"].GetUint() > 0);

			size_t currentTime = response["current_time"].GetUint();

			// Now, stop the game, wait a little while, and then confirm that
			// the timer hasn't advanced.
			response = stopGame(id);
			std::this_thread::sleep_for(threadSleepTime);
			response = getTime(id);

			// Sometimes, we call stopGame() just as the timer is about to
			// advance, so we have to accept it advancing by one as a possible
			// result.
			bool validTime = currentTime == response["current_time"].GetUint() || currentTime == response["current_time"].GetUint() - 1;

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(validTime);

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getStatistics()") {

		SUBCASE("Missing required game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("action", "statistics", request.GetAllocator());

			// Step 1: Test with no args member
			rapidjson::Document response = GameController::get()->getStatistics(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			// Step 2: test with empty args member
			request.AddMember("args", rapidjson::Value(rapidjson::kObjectType).Move(), request.GetAllocator());

			response = GameController::get()->getStatistics(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("No games, non-existent game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getStatistics(0);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("One game, non-existent game id") {

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

			size_t id = response["id"].GetUint();

			response = getStatistics(id + 1);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Checking created, current_time, and is_running stats") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			static std::chrono::milliseconds threadSleepTime(tickInterval * 2);
			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			// Verify that game is created in a stopped state by default by
			// waiting a little while and seeing that the game's timer
			// doesn't advance.
			std::this_thread::sleep_for(threadSleepTime);
			response = getStatistics(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("created"));
			CHECK(response["created"].IsString());

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(false == response["is_running"].GetBool());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(0 == response["current_time"].GetUint());

			std::string createdAt = response["created"].GetString();

			// Now, start the game and wait a little while before getting the
			// time and confirm that the timer has advanced.
			response = startGame(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			std::this_thread::sleep_for(threadSleepTime);
			response = getStatistics(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("created"));
			CHECK(response["created"].IsString());
			CHECK(0 == createdAt.compare(response["created"].GetString()));

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(true == response["is_running"].GetBool());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(response["current_time"].GetUint() > 0);

			size_t currentTime = response["current_time"].GetUint();

			// Now, stop the game, wait a little while, and then confirm that
			// the timer hasn't advanced and that the game is once more
			// reporting as stopped.
			response = stopGame(id);
			std::this_thread::sleep_for(threadSleepTime);
			response = getStatistics(id);

			// Sometimes, we call stopGame() just as the timer is about to
			// advance, so we have to accept it advancing by one as a possible
			// result.
			bool validTime = currentTime == response["current_time"].GetUint() || currentTime == response["current_time"].GetUint() - 1;

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("created"));
			CHECK(response["created"].IsString());
			CHECK(0 == createdAt.compare(response["created"].GetString()));

			CHECK(response.HasMember("is_running"));
			CHECK(response["is_running"].IsBool());
			CHECK(false == response["is_running"].GetBool());

			CHECK(response.HasMember("current_time"));
			CHECK(response["current_time"].IsUint());
			CHECK(validTime);

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Checking number of players") {

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

			size_t id = response["id"].GetUint();

			// First, verify that the game starts off with 0 players
			response = getStatistics(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("players"));
			CHECK(response["players"].IsUint());
			CHECK(0 == response["players"].GetUint());

			// Next, create a player and verify that it now shows up in the stats
			response = createPlayer(id, "player_name");

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getStatistics(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("players"));
			CHECK(response["players"].IsUint());
			CHECK(1 == response["players"].GetUint());

			// Finally, remove the player and verify that the number drops back to zero
			response = destroyPlayer(id, "player_name");

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = getStatistics(id);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("players"));
			CHECK(response["players"].IsUint());
			CHECK(0 == response["players"].GetUint());

			destroyGameXML();
			destroyConfig();
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): getDumped()") {

		SUBCASE("State disabled, without game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getDumped();

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Response::STATE_DISABLED).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("State disabled, with non-existent game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = getDumped(0);

			CHECK(trogdor::isAscii(JSON::serialize(response)));

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(Response::STATE_DISABLED).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("State disabled, with existing game id") {

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			// TODO

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id") {

			// TODO
		}

		SUBCASE("No game id: returns list of dumped games (no dumped games exist)") {

			// TODO
		}

		SUBCASE("No game id: returns list of dumped games (one dumped game exists)") {

			// TODO: three dumped games (one is empty directory, one is directory
			// with subdirectory called "meta", and the last is a valid game. Should
			// only get one game back in list.)
		}

		SUBCASE("With game id: game id doesn't exist") {

			// TODO
		}

		SUBCASE("With game id: game id directory is empty (should treat this as GameNotFound)") {

			// TODO
		}

		SUBCASE("With game id: directory is not readable. Should return 500 Internal Error.") {

			// TODO
		}

		SUBCASE("With game id: returns list of dump slots") {

			// TODO
		}
	}

	TEST_CASE("GameController (scopes/game.cpp): dumpGame()") {

		// TODO
	}

	TEST_CASE("GameController (scopes/game.cpp): restoreGame()") {

		// TODO
	}
}
