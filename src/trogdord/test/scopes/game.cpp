#include <doctest.h>

#include "../../include/json.h"
#include "../../include/request.h"
#include "../../include/response.h"
#include "../../include/config.h"
#include "../../include/filesystem.h"

#include "../../include/scopes/game.h"


// Full path to our unit test's ini file
std::string iniFilename;

// Full path to where we're storing our unit test xml files
std::string gameXMLLocation;

// Full path to our unit test's game.xml file
std::string gameXMLFilename;

// The XML filename of our game.xml file without the prepended path
std::string gameXMLRelativeFilename = "game.xml";

// Sets up a test game.xml file for our unit test
void initGameXML() {

	gameXMLLocation = STD_FILESYSTEM::temp_directory_path().string() +
		STD_FILESYSTEM::path::preferred_separator + "trogtest";

	STD_FILESYSTEM::create_directory(gameXMLLocation);

	gameXMLFilename = gameXMLLocation + STD_FILESYSTEM::path::preferred_separator + gameXMLRelativeFilename;
	std::ofstream gameXMLFile (gameXMLFilename, std::ofstream::out);

	gameXMLFile << "<?xml version=\"1.0\"?>\n" << std::endl;
	gameXMLFile << "<game>\n" << std::endl;

	gameXMLFile << "\t<objects>\n" << std::endl;
	gameXMLFile << "\t\t<object name=\"candle\">\n" << std::endl;
	gameXMLFile << "\t\t\t<description>Test</description>\n" << std::endl;
	gameXMLFile << "\t\t</object>\n" << std::endl;
	gameXMLFile << "\t</objects>\n" << std::endl;

	gameXMLFile << "\t<rooms>\n" << std::endl;
	gameXMLFile << "\t\t<room name=\"start\">\n" << std::endl;
	gameXMLFile << "\t\t\t<description>Test</description>\n" << std::endl;
	gameXMLFile << "\t\t\t<contains>\n" << std::endl;
	gameXMLFile << "\t\t\t\t<object>candle</object>\n" << std::endl;
	gameXMLFile << "\t\t\t</contains>\n" << std::endl;
	gameXMLFile << "\t\t</room>\n" << std::endl;
	gameXMLFile << "\t</rooms>\n" << std::endl;

	gameXMLFile << "</game>" << std::endl;

	gameXMLFile.close();
}

// Destroys our unit test's game.xml file
void destroyGameXML() {

	STD_FILESYSTEM::remove_all(gameXMLLocation);
}

// Initializes an ini file with the necessary configuration in /tmp. This must
// be called *AFTER* a call to initGameXML().
void initConfig() {

	iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
	std::ofstream iniFile (iniFilename, std::ofstream::out);

	iniFile << "[resources]\ndefinitions_path=" << gameXMLLocation << std::endl;
	iniFile.close();

	Config::get()->load(iniFilename);
}

// Destroys our unit test's ini file
void destroyConfig() {

	STD_FILESYSTEM::remove(iniFilename);
}

TEST_SUITE("GameController (scopes/game.cpp)") {

	TEST_CASE("GameController (scopes/game.cpp): getDefinitionList()") {

		initGameXML();
		initConfig();

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", "get", request.GetAllocator());
		request.AddMember("scope", "global", request.GetAllocator());
		request.AddMember("action", "statistics", request.GetAllocator());

		rapidjson::Document response = GameController::get()->getDefinitionList(request);

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

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", "test", request.GetAllocator());

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

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

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INTERNAL_ERROR == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation is successful without meta") {

			const char *gameName = "myGame";

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("name", rapidjson::StringRef(gameName), request.GetAllocator());
			args.AddMember(
				"definition",
				rapidjson::StringRef(gameXMLRelativeFilename.c_str()),
				request.GetAllocator()
			);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->createGame(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));

			// Only try to verify that the game was created if we got a
			// success status back from the last request
			if (response.HasMember("id")) {

				CHECK(response["id"].IsUint());
				size_t id = response["id"].GetUint();

				rapidjson::Document getGameRequest(rapidjson::kObjectType);
				rapidjson::Value getGameArgs(rapidjson::kObjectType);

				getGameArgs.AddMember("id", id, getGameRequest.GetAllocator());

				getGameRequest.AddMember("method", "get", getGameRequest.GetAllocator());
				getGameRequest.AddMember("scope", "game", getGameRequest.GetAllocator());
				getGameRequest.AddMember("args", getGameArgs, getGameRequest.GetAllocator());

				response = GameController::get()->getGame(getGameRequest);

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

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				CHECK(response.HasMember("meta"));
				CHECK(response["meta"].IsObject());
				CHECK(response["meta"].MemberBegin() == response["meta"].MemberEnd());
			}

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation is successful with valid meta") {

			const char *gameName = "myGame";
			const char *argTitle = "My Title";
			const size_t argPositiveNumber = 1;
			const int argNegativeNumber = -1;
			const bool argBool = false;

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

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));

			// Only try to verify that the game was created if we got a
			// success status back from the last request
			if (response.HasMember("id")) {

				CHECK(response["id"].IsUint());
				size_t id = response["id"].GetUint();

				rapidjson::Document getGameRequest(rapidjson::kObjectType);
				rapidjson::Value getGameArgs(rapidjson::kObjectType);

				getGameArgs.AddMember("id", id, getGameRequest.GetAllocator());

				getGameRequest.AddMember("method", "get", getGameRequest.GetAllocator());
				getGameRequest.AddMember("scope", "game", getGameRequest.GetAllocator());
				getGameRequest.AddMember("args", getGameArgs, getGameRequest.GetAllocator());

				response = GameController::get()->getGame(getGameRequest);

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
			}

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation is successful with invalid meta (object)") {

			const char *gameName = "myGame";

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

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::INVALID_META).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Game creation is successful with invalid meta (array)") {

			const char *gameName = "myGame";

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

	// TODO: next should be destroyGame()

	// TODO: I can do the rest after this, since I've already validated that
	// creating an destroying a game work
}
