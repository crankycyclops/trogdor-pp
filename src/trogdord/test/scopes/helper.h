#include <unordered_map>

#include "../../include/json.h"
#include "../../include/request.h"
#include "../../include/response.h"
#include "../../include/config.h"
#include "../../include/filesystem.h"
#include "../../include//gamecontainer.h"

#include "../../include/scopes/game.h"
#include "../../include/scopes/player.h"


// Number of ms between clock ticks (make this value small enough to ensure
// time-dependent unit tests finish quickly, but not so small that the Timer
// becomes unstable.)
constexpr size_t tickInterval = 5;

// Full path to our unit test's ini file
std::string iniFilename;

// Full path to where we're storing our unit test xml files
std::string gameXMLLocation;

// Full path to our unit test's game.xml file
std::string gameXMLFilename;

// The XML filename of our game.xml file without the prepended path
std::string gameXMLRelativeFilename = "game.xml";

// Test meta data
std::unordered_map<std::string, std::string> testMeta = {
	{"key1", "value1"},
	{"key2", "value2"}
};

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
rapidjson::Document createGame(
	const char *name,
	const char *definition,
	std::unordered_map<std::string, std::string> meta = {}
) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Value args(rapidjson::kObjectType);

	args.AddMember("name", rapidjson::StringRef(name), request.GetAllocator());
	args.AddMember(
		"definition",
		rapidjson::StringRef(definition),
		request.GetAllocator()
	);

	if (meta.size()) {

		for (const auto &metaVal: meta) {

			rapidjson::Value key(rapidjson::kStringType);
			rapidjson::Value value(rapidjson::kStringType);

			key.SetString(rapidjson::StringRef(metaVal.first.c_str()), request.GetAllocator());
			value.SetString(rapidjson::StringRef(metaVal.second.c_str()), request.GetAllocator());

			args.AddMember(
				key.Move(),
				value.Move(),
				request.GetAllocator()
			);
		}
	}

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

// Gets the details of an existing game
rapidjson::Document getGame(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Value args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getGame(request);
}

// Gets some or all meta data associated with an existing game
rapidjson::Document getMeta(size_t id, std::vector<const char *> keys = {}) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Value args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	if (keys.size()) {

		args.AddMember("meta", rapidjson::Value(rapidjson::kArrayType), request.GetAllocator());

		for (const auto &key: keys) {
			rapidjson::Value keyVal(rapidjson::kStringType);
			keyVal.SetString(rapidjson::StringRef(key), request.GetAllocator());
			args["meta"].PushBack(keyVal.Move(), request.GetAllocator());
		}
	}

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getMeta(request);
}

// Gets some or all meta data associated with an existing game
rapidjson::Document setMeta(size_t id, std::unordered_map<std::string, const char *> meta = {}) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Value args(rapidjson::kObjectType);
	rapidjson::Value metaArg(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	if (meta.size()) {

		for (const auto &metaVal: meta) {
			metaArg[metaVal.first.c_str()] = rapidjson::StringRef(metaVal.second);
		}
	}

	args.AddMember("meta", metaArg, request.GetAllocator());

	request.AddMember("method", "set", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "meta", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->setMeta(request);
}

// filters should be a valid JSON string
rapidjson::Document getGameList(
	std::vector<const char *> metaKeys = {},
	const char *filters = nullptr
) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	if (metaKeys.size()) {

		rapidjson::Value meta(rapidjson::kArrayType);

		for (const auto &key: metaKeys) {
			meta.PushBack(rapidjson::StringRef(key), request.GetAllocator());
		}

		args.AddMember("include_meta", meta, request.GetAllocator());
	}

	if (filters) {

		rapidjson::Document filterArg;
		rapidjson::Value filterValue(rapidjson::kObjectType);

		// I'm not sure why I have to do this, but if I attempt to use
		// filterArg directly, I get corrupted memory. Whatever, man.
		filterArg.Parse(filters);
		filterValue.CopyFrom(filterArg, request.GetAllocator());

		args.AddMember("filters", filterValue, request.GetAllocator());
	}

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "list", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getGameList(request);
}

// Starts a game
rapidjson::Document startGame(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	request.AddMember("method", "set", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "start", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->startGame(request);
}

// Starts a game
rapidjson::Document stopGame(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	request.AddMember("method", "set", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "stop", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->stopGame(request);
}

// Returns whether or not the game is running
rapidjson::Document isRunning(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "is_running", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getIsRunning(request);
}

// Returns the current time in a game
rapidjson::Document getTime(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "time", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getTime(request);
}

// Returns a game's statistics
rapidjson::Document getStatistics(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "statistics", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getStatistics(request);
}

// Creates a player
rapidjson::Document createPlayer(size_t gameId, const char *name) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());
	args.AddMember("name", rapidjson::StringRef(name), request.GetAllocator());

	request.AddMember("method", "post", request.GetAllocator());
	request.AddMember("scope", "player", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return PlayerController::get()->createPlayer(request);
}

// Deletes a player
rapidjson::Document destroyPlayer(size_t gameId, const char *name) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());
	args.AddMember("name", rapidjson::StringRef(name), request.GetAllocator());

	request.AddMember("method", "delete", request.GetAllocator());
	request.AddMember("scope", "player", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return PlayerController::get()->destroyPlayer(request);
}
