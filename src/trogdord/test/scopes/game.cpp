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

	gameXMLFile << "\t<meta>\n" << std::endl;
	gameXMLFile << "\t\t<title>Test Title</title>\n" << std::endl;
	gameXMLFile << "\t</meta>\n" << std::endl;

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

		SUBCASE("Optional meta argument passed as incorrect type (array)") {

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);
			rapidjson::Value meta(rapidjson::kArrayType);

			args.AddMember("name", "test", request.GetAllocator());
			args.AddMember("definition", rapidjson::StringRef(gameXMLRelativeFilename.c_str()), request.GetAllocator());
			args.AddMember("metakey", meta, request.GetAllocator());

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

		// TODO: finish
	}

	// TODO: next should be destroyGame()

	// TODO: I can do the rest after this, since I've already validated that
	// creating an destroying a game work
}
