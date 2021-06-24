#ifndef TROGDORD_TEST_HELPER_H
#define TROGDORD_TEST_HELPER_H


#include <fstream>
#include <unordered_set>
#include <unordered_map>

#include <trogdor/utility.h>

#include <trogdord/json.h>
#include <trogdord/request.h>
#include <trogdord/response.h>
#include <trogdord/config.h>
#include <trogdord/filesystem.h>
#include <trogdord//gamecontainer.h>

#include <trogdord/scopes/game.h>
#include <trogdord/scopes/player.h>
#include <trogdord/scopes/resource.h>
#include <trogdord/scopes/object.h>
#include <trogdord/scopes/room.h>
#include <trogdord/scopes/creature.h>


// Number of ms between clock ticks (make this value small enough to ensure
// time-dependent unit tests finish quickly, but not so small that the Timer
// becomes unstable.)
constexpr size_t tickInterval = 5;

// Full path to our unit test's ini file
static std::string iniFilename;

// Full path to where we're storing our unit test xml files
static std::string gameXMLLocation;

// Full path to our unit test's game.xml file
static std::string gameXMLFilename;

// The XML filename of our game.xml file without the prepended path
static std::string gameXMLRelativeFilename = "game.xml";

// Test meta data
static std::unordered_map<std::string, std::string> testMeta = {
	{"key1", "value1"},
	{"key2", "value2"}
};

// Names for each sample entity in the game created by initGameXML()
static const char *resourceName = "gold";
static const char *creatureName = "trogdor";
static const char *objectName = "candle";
static const char *roomName = "start";

// List of all entities of a particular type in the game created by initGameXML()
static const std::unordered_set<std::string> entities = {
	creatureName,
	objectName,
	roomName,
	resourceName
};

static const std::unordered_set<std::string> resources = {
	resourceName
};

static const std::unordered_set<std::string> tangibles = {
	creatureName,
	objectName,
	roomName
};

static const std::unordered_set<std::string> objects = {
	objectName
};

static const std::unordered_set<std::string> places = {
	roomName
};

static const std::unordered_set<std::string> rooms = {
	roomName
};

static const std::unordered_set<std::string> things = {
	creatureName,
	objectName
};

static const std::unordered_set<std::string> beings = {
	creatureName
};

static const std::unordered_set<std::string> creatures = {
	creatureName
};

// Sets up a test game.xml file for our unit test
inline void initGameXML() {

	gameXMLLocation = STD_FILESYSTEM::temp_directory_path().string() +
		STD_FILESYSTEM::path::preferred_separator + "trogtest";

	STD_FILESYSTEM::create_directory(gameXMLLocation);

	gameXMLFilename = gameXMLLocation + STD_FILESYSTEM::path::preferred_separator + gameXMLRelativeFilename;
	std::ofstream gameXMLFile (gameXMLFilename, std::ofstream::out);

	gameXMLFile << "<?xml version=\"1.0\"?>\n" << std::endl;
	gameXMLFile << "<game>\n" << std::endl;

	gameXMLFile << "\t<creatures>\n" << std::endl;
	gameXMLFile << "\t\t<creature name=\"" << creatureName << "\">\n" << std::endl;
	gameXMLFile << "\t\t\t<description>He be cray cray.</description>\n" << std::endl;
	gameXMLFile << "\t\t</creature>\n" << std::endl;
	gameXMLFile << "\t</creatures>\n" << std::endl;

	gameXMLFile << "\t<objects>\n" << std::endl;
	gameXMLFile << "\t\t<object name=\"" << objectName << "\">\n" << std::endl;
	gameXMLFile << "\t\t\t<description>Test</description>\n" << std::endl;
	gameXMLFile << "\t\t</object>\n" << std::endl;
	gameXMLFile << "\t</objects>\n" << std::endl;

	gameXMLFile << "\t<resources>\n" << std::endl;
	gameXMLFile << "\t\t<resource name=\"" << resourceName << "\">\n" << std::endl;
	gameXMLFile << "\t\t\t<description>It's shiny.</description>\n" << std::endl;
	gameXMLFile << "\t\t</resource>\n" << std::endl;
	gameXMLFile << "\t</resources>\n" << std::endl;

	gameXMLFile << "\t<rooms>\n" << std::endl;
	gameXMLFile << "\t\t<room name=\"" << roomName << "\">\n" << std::endl;
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
inline void destroyGameXML() {

	STD_FILESYSTEM::remove_all(gameXMLLocation);
}

// Initializes an ini file with the necessary configuration in /tmp. This must
// be called *AFTER* a call to initGameXML().
inline void initConfig(
	bool redisSupport = false,
	std::optional<bool> stateEnabled = std::nullopt,
	std::optional<std::string> statePath = std::nullopt
) {

	iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
	std::ofstream iniFile (iniFilename, std::ofstream::out);

	if (stateEnabled || statePath) {

		iniFile << "[state]\n";

		if (stateEnabled) {
			iniFile << "enabled=" << (*stateEnabled ? "true" : "false") << '\n';
		}

		if (statePath) {
			iniFile << "save_path=" << ((*statePath).length() ? *statePath : "\"\"") << '\n';
		}

		iniFile << "\n" << std::endl;
	}

	if (redisSupport) {
		iniFile << "[output]\ndriver=redis\n\n" << std::endl;
	}

	iniFile << "[resources]\ndefinitions_path=" << gameXMLLocation << std::endl;
	iniFile.close();

	Config::get()->load(iniFilename);
}

// Destroys our unit test's ini file
inline void destroyConfig() {

	STD_FILESYSTEM::remove(iniFilename);
}

// Creates a game
inline rapidjson::Document createGame(
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
inline rapidjson::Document destroyGame(size_t id, std::optional<bool> deleteDump = std::nullopt) {

	rapidjson::Document deleteRequest(rapidjson::kObjectType);
	rapidjson::Value deleteArgs(rapidjson::kObjectType);

	deleteArgs.AddMember("id", id, deleteRequest.GetAllocator());

	if (deleteDump) {
		deleteArgs.AddMember("delete_dump", *deleteDump, deleteRequest.GetAllocator());
	}

	deleteRequest.AddMember("method", "delete", deleteRequest.GetAllocator());
	deleteRequest.AddMember("scope", "game", deleteRequest.GetAllocator());
	deleteRequest.AddMember("args", deleteArgs, deleteRequest.GetAllocator());

	return GameController::get()->destroyGame(deleteRequest);
}

// Gets the details of an existing game
inline rapidjson::Document getGame(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Value args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getGame(request);
}

// Gets some or all meta data associated with an existing game
inline rapidjson::Document getMeta(size_t id, std::vector<const char *> keys = {}) {

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
inline rapidjson::Document setMeta(size_t id, std::unordered_map<std::string, const char *> meta = {}) {

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
inline rapidjson::Document getGameList(
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
inline rapidjson::Document startGame(size_t id) {

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
inline rapidjson::Document stopGame(size_t id) {

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
inline rapidjson::Document isRunning(size_t id) {

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
inline rapidjson::Document getTime(size_t id) {

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
inline rapidjson::Document getStatistics(size_t id) {

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
inline rapidjson::Document createPlayer(size_t gameId, const char *name) {

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
inline rapidjson::Document destroyPlayer(
	size_t gameId,
	const char *name,
	const char *message = nullptr
) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());
	args.AddMember("name", rapidjson::StringRef(name), request.GetAllocator());

	if (message) {
		args.AddMember("message", rapidjson::StringRef(message), request.GetAllocator());
	}

	request.AddMember("method", "delete", request.GetAllocator());
	request.AddMember("scope", "player", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return PlayerController::get()->destroyPlayer(request);
}

// Get an entity.
inline rapidjson::Document getEntity(
	size_t gameId,
	const char *name,
	const char *type = "entity"
) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());
	args.AddMember("name", rapidjson::StringRef(name), request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", rapidjson::StringRef(type), request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	// This is kind of lame, but it works...
	if (0 == std::string(type).compare("entity")) {
		return EntityController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("resource")) {
		return ResourceController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("tangible")) {
		return TangibleController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("thing")) {
		return ThingController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("object")) {
		return ObjectController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("place")) {
		return PlaceController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("room")) {
		return RoomController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("being")) {
		return BeingController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("creature")) {
		return CreatureController::get()->getEntity(request);
	}

	else if (0 == std::string(type).compare("player")) {
		return PlayerController::get()->getEntity(request);
	}

	else {

		std::string errorMsg = "Helper getEntity() not defined for entity type '";
		errorMsg += type;
		errorMsg += "'";

		throw std::runtime_error(errorMsg);
	}
}

// Get all entities in a game.
inline rapidjson::Document getEntityList(size_t gameId, const char *type = "entity") {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", rapidjson::StringRef(type), request.GetAllocator());
	request.AddMember("action", "list", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	// This is kind of lame, but it works...
	if (0 == std::string(type).compare("entity")) {
		return EntityController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("resource")) {
		return ResourceController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("tangible")) {
		return TangibleController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("thing")) {
		return ThingController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("object")) {
		return ObjectController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("place")) {
		return PlaceController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("room")) {
		return RoomController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("being")) {
		return BeingController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("creature")) {
		return CreatureController::get()->getEntityList(request);
	}

	else if (0 == std::string(type).compare("player")) {
		return PlayerController::get()->getEntityList(request);
	}

	else {

		std::string errorMsg = "Helper getEntityList() not defined for entity type '";
		errorMsg += type;
		errorMsg += "'";

		throw std::runtime_error(errorMsg);
	}
}

// Get all output messages for an entity on the specified channel.
inline rapidjson::Document getOutput(
	size_t gameId,
	const char *entityName,
	const char *channel
) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());
	args.AddMember("name", rapidjson::StringRef(entityName), request.GetAllocator());
	args.AddMember("channel", rapidjson::StringRef(channel), request.GetAllocator());

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "entity", request.GetAllocator());
	request.AddMember("action", "output", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return EntityController::get()->getOutput(request);
}

// Append a message to an entity's output on the specified channel.
inline rapidjson::Document appendOutput(
	size_t gameId,
	const char *entityName,
	const char *message,
	const char *channel = nullptr
) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());
	args.AddMember("name", rapidjson::StringRef(entityName), request.GetAllocator());
	args.AddMember("message", rapidjson::StringRef(message), request.GetAllocator());

	if (channel) {
		args.AddMember("channel", rapidjson::StringRef(channel), request.GetAllocator());
	}

	request.AddMember("method", "post", request.GetAllocator());
	request.AddMember("scope", "entity", request.GetAllocator());
	request.AddMember("action", "output", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return EntityController::get()->appendOutput(request);
}

// Post input to a player's input stream
inline rapidjson::Document postInput(
	size_t gameId,
	const char *entityName,
	const char *command
) {

	rapidjson::Document request(rapidjson::kObjectType);
	rapidjson::Document args(rapidjson::kObjectType);

	args.AddMember("game_id", gameId, request.GetAllocator());
	args.AddMember("name", rapidjson::StringRef(entityName), request.GetAllocator());
	args.AddMember("command", rapidjson::StringRef(command), request.GetAllocator());

	request.AddMember("method", "post", request.GetAllocator());
	request.AddMember("scope", "player", request.GetAllocator());
	request.AddMember("action", "input", request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return PlayerController::get()->postInput(request);
}

// Dumps the specified game.
inline rapidjson::Document dumpGame(size_t id) {

	rapidjson::Document request(rapidjson::kObjectType);

	request.AddMember("method", "post", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "dump", request.GetAllocator());

	rapidjson::Value args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());
	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->dumpGame(request);
}

// Restores the specified game.
inline rapidjson::Document restoreGame(size_t id, std::optional<size_t> slot = std::nullopt) {

	rapidjson::Document request(rapidjson::kObjectType);

	request.AddMember("method", "post", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "restore", request.GetAllocator());

	rapidjson::Value args(rapidjson::kObjectType);

	args.AddMember("id", id, request.GetAllocator());

	if (slot) {
		args.AddMember("slot", *slot, request.GetAllocator());
	}

	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->restoreGame(request);
}

// Returns data about a game's entire dump history or a specific dump slot,
// depending on whether or not a slot is passed in as an optional additional
// argument.
inline rapidjson::Document getDump(
	size_t id,
	std::optional<size_t> slot = std::nullopt
) {

	rapidjson::Value args(rapidjson::kObjectType);
	rapidjson::Document request(rapidjson::kObjectType);

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "dump", request.GetAllocator());

	args.AddMember("id", id, request.GetAllocator());

	if (slot) {
		args.AddMember("slot", *slot, request.GetAllocator());
	}

	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->getDump(request);
}

// Depending on whether or not an id is passed in, either returns a list of all
// dumped games or a list of all save slots within a dumped game.
inline rapidjson::Document getDumpList(
	std::optional<size_t> id = std::nullopt
) {

	rapidjson::Document request(rapidjson::kObjectType);

	request.AddMember("method", "get", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "dumplist", request.GetAllocator());

	if (id) {

		rapidjson::Value args(rapidjson::kObjectType);

		args.AddMember("id", *id, request.GetAllocator());
		request.AddMember("args", args, request.GetAllocator());
	}

	return GameController::get()->getDumpList(request);
}

// Destroys a game's entire dump history, or just a single dump slot
inline rapidjson::Document destroyDump(
	size_t id,
	std::optional<size_t> slot = std::nullopt
) {

	rapidjson::Value args(rapidjson::kObjectType);
	rapidjson::Document request(rapidjson::kObjectType);

	request.AddMember("method", "delete", request.GetAllocator());
	request.AddMember("scope", "game", request.GetAllocator());
	request.AddMember("action", "dump", request.GetAllocator());

	args.AddMember("id", id, request.GetAllocator());

	if (slot) {
		args.AddMember("slot", *slot, request.GetAllocator());
	}

	request.AddMember("args", args, request.GetAllocator());

	return GameController::get()->destroyDump(request);
}


#endif // TROGDORD_TEST_HELPER_H
