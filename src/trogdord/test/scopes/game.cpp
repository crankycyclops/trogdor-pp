#include <doctest.h>
#include <trogdor/utility.h>

#include "../../include/json.h"
#include "../../include/request.h"
#include "../../include/response.h"
#include "../../include/config.h"
#include "../../include/filesystem.h"

#include "../../include/scopes/game.h"
#include "../../include//gamecontainer.h"


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

// Creates a game
rapidjson::Document createGame(const char *name, const char *definition) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Value args(rapidjson::kObjectType);

	args.AddMember("name", rapidjson::StringRef(name), request.GetAllocator());
	args.AddMember(
		"definition",
		rapidjson::StringRef(definition),
		request.GetAllocator()
	);

	request.AddMember("method", "post", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->createGame(request);
}

// Destroys a game
rapidjson::Document destroyGame(size_t id) {

	rapidjson::Document deleteRequest(rapidjson::kObjectType);
	rapidjson::Value deleteArgs(rapidjson::kObjectType);

	deleteArgs.AddMember("id", id, deleteRequest.GetAllocator());

	deleteRequest.AddMember("method", "delete", deleteRequest.GetAllocator());
	deleteRequest.AddMember("scope", "game", deleteRequest.GetAllocator());
	deleteRequest.AddMember("args", deleteArgs, deleteRequest.GetAllocator());

	return GameController::get()->destroyGame(deleteRequest);
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

			const char *gameName = "myGame";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

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

		SUBCASE("Game creation fails because of invalid meta (object)") {

			const char *gameName = "myGame";

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

			const char *gameName = "myGame";

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

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));
		}

		SUBCASE("Successful destruction of game") {

			// Step 1: create a game and store the ID
			const char *gameName = "myGame";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));

			if (response.HasMember("id")) {

				CHECK(response["id"].IsUint());
				size_t id = response["id"].GetUint();

				// Make sure getGame() can find it
				rapidjson::Document getRequest(rapidjson::kObjectType);
				rapidjson::Value getArgs(rapidjson::kObjectType);

				getArgs.AddMember("id", id, getRequest.GetAllocator());

				getRequest.AddMember("method", "get", getRequest.GetAllocator());
				getRequest.AddMember("scope", "game", getRequest.GetAllocator());
				getRequest.AddMember("args", getArgs, getRequest.GetAllocator());

				response = GameController::get()->getGame(getRequest);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				// Delete the game
				response = destroyGame(id);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				// Make sure getGame() can no longer find it
				response = GameController::get()->getGame(getRequest);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

				CHECK(response.HasMember("message"));
				CHECK(response["message"].IsString());
				CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

				// Make sure destroyGame() returns 404 if we call it again with the same id
				response = destroyGame(id);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

				CHECK(response.HasMember("message"));
				CHECK(response["message"].IsString());
				CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));
			}

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

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_INVALID == response["status"].GetUint());
			CHECK( 0 == std::string(Request::MISSING_GAME_ID).compare(response["message"].GetString()));

			destroyGameXML();
			destroyConfig();
		}

		SUBCASE("Invalid game id") {

			// Step 1: test with no games running

			const char *gameName = "myGame";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document request(rapidjson::kObjectType);
			rapidjson::Value args(rapidjson::kObjectType);

			args.AddMember("id", 1, request.GetAllocator());

			request.AddMember("method", "get", request.GetAllocator());
			request.AddMember("scope", "game", request.GetAllocator());
			request.AddMember("args", args, request.GetAllocator());

			rapidjson::Document response = GameController::get()->getGame(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());
			CHECK( 0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));

			// Step 2: test with a game running and an invalid id

			response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			request["args"]["id"].SetUint(id + 1);
			response = GameController::get()->getGame(request);

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

			const char *gameName = "myGame";

			GameContainer::get()->reset();

			initGameXML();
			initConfig();

			rapidjson::Document response = createGame(gameName, gameXMLRelativeFilename.c_str());

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			CHECK(response.HasMember("id"));
			CHECK(response["id"].IsUint());

			size_t id = response["id"].GetUint();

			rapidjson::Document getRequest(rapidjson::kObjectType);
			rapidjson::Value getArgs(rapidjson::kObjectType);

			getArgs.AddMember("id", id, getRequest.GetAllocator());

			getRequest.AddMember("method", "get", getRequest.GetAllocator());
			getRequest.AddMember("scope", "game", getRequest.GetAllocator());
			getRequest.AddMember("args", getArgs, getRequest.GetAllocator());

			response = GameController::get()->getGame(getRequest);

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

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

			response = GameController::get()->getGame(getRequest);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_NOT_FOUND == response["status"].GetUint());

			CHECK(response.HasMember("message"));
			CHECK(response["message"].IsString());
			CHECK(0 == std::string(GameController::GAME_NOT_FOUND).compare(response["message"].GetString()));
		}
	}
}
