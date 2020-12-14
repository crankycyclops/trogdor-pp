#include <chrono>
#include <fstream>
#include <cstdint>

#include <trogdor/parser/parsers/xmlparser.h>
#include <trogdor/serial/serializable.h>

#include "include/config.h"
#include "include/filesystem.h"
#include "include/serial/drivermap.h"
#include "include/io/iostream/serverout.h"
#include "include/exception/serverexception.h"

#include "include/gamewrapper.h"


GameWrapper::GameWrapper(
	size_t gid,
	std::string &definitionPath,
	std::string &name,
	std::unordered_map<std::string, std::string> meta
): id(gid), name(name), definition(STD_FILESYSTEM::path(definitionPath).filename()) {

	definitionPath = Filesystem::getFullDefinitionsPath(definitionPath);

	// TODO: will need better and more specific error logging than just a
	// simple copy of the global server error logger
	gamePtr = std::make_unique<trogdor::Game>(
		Config::get()->err().copy()
	);

	std::unique_ptr<trogdor::XMLParser> parser = std::make_unique<trogdor::XMLParser>(
		gamePtr->makeInstantiator(),
		gamePtr->getVocabulary(),
		gamePtr->err()
	);

	if (!gamePtr->initialize(parser.get(), definitionPath, true)) {
		throw ServerException("failed to initialize game");
	}

	// Record when the game was created
	created = std::time(nullptr);

	// If any custom meta data was specified, set it
	for (auto &pair: meta) {
		gamePtr->setMeta(pair.first, pair.second);
	}
}

/*****************************************************************************/

GameWrapper::GameWrapper(const STD_FILESYSTEM::path &p): gamePtr(nullptr) {

	std::string idPath = p;
	std::string idStr = p.filename();
	std::set<size_t> timestamps;

	// Find the latest timestamp and restore that version of the game
	getDumpedGameSlots(timestamps, idPath);

	std::string timestampStr = std::to_string(*timestamps.rbegin());
	std::string metaPath = idPath + STD_FILESYSTEM::path::preferred_separator +
		timestampStr + STD_FILESYSTEM::path::preferred_separator + "meta";
	std::string gamePath = idPath + STD_FILESYSTEM::path::preferred_separator +
		timestampStr + STD_FILESYSTEM::path::preferred_separator + "game";

	if (
		!STD_FILESYSTEM::exists(metaPath) ||
		STD_FILESYSTEM::is_directory(metaPath) ||
		!STD_FILESYSTEM::exists(gamePath) ||
		STD_FILESYSTEM::is_directory(gamePath)
	) {
		throw ServerException(
			std::string("Attempted to restore an invalidly dumped game with id ") + idStr
		);
	}

	auto &serialDriver =
		serial::DriverMap::get(Config::get()->getString(Config::CONFIG_KEY_STATE_FORMAT));

	ifstream metaFile(metaPath);
	ifstream gameFile(gamePath);

	ostringstream metaStr;
	ostringstream gameStr;

	metaStr << metaFile.rdbuf();
	gameStr << gameFile.rdbuf();

	std::shared_ptr<trogdor::serial::Serializable> metaData = serialDriver->deserialize(metaStr.str());
	std::shared_ptr<trogdor::serial::Serializable> gameData = serialDriver->deserialize(gameStr.str());

	gameMutex.lock();

	// Deserialize meta data associated with the GameWrapper instance
	id = std::get<size_t>(*metaData->get("id"));
	name = std::get<std::string>(*metaData->get("name"));
	definition = std::get<std::string>(*metaData->get("definition"));
	created = static_cast<time_t>(std::get<size_t>(*metaData->get("created")));

	gamePtr = std::make_unique<trogdor::Game>(
		gameData,
		Config::get()->err().copy(),
		[&](trogdor::Game *game) -> std::unique_ptr<trogdor::Trogout> {
			return std::make_unique<ServerOut>(id);
		}, [&](trogdor::Game *game) -> std::unique_ptr<trogdor::Trogerr> {
			return Config::get()->err().copy();
		}
	);

	for (auto &player: gamePtr->getPlayers()) {
		static_cast<ServerOut *>(&player.second->out())->setEntity(player.second.get());
	}

	gameMutex.unlock();
}

/*****************************************************************************/

void GameWrapper::getDumpedGameSlots(std::set<size_t> &slots, std::string gameIdPath) {

	for (const auto &subdir: STD_FILESYSTEM::directory_iterator(gameIdPath)) {

		std::string timestamp = subdir.path().filename();

		// Skip over obviously invalid files and directories
		if (!trogdor::isValidInteger(timestamp)) {
			continue;
		}

		else if (!STD_FILESYSTEM::is_directory(subdir.path())) {
			continue;
		}

		#if SIZE_MAX == UINT64_MAX
			slots.insert(std::stoull(timestamp));
		#else
			slots.insert(std::stoul(timestamp));
		#endif
	}
}

/*****************************************************************************/

std::shared_ptr<trogdor::serial::Serializable> GameWrapper::serializeMeta() {

	std::shared_ptr<trogdor::serial::Serializable> meta =
		std::make_shared<trogdor::serial::Serializable>();

	meta->set("id", id);
	meta->set("name", name);
	meta->set("definition", definition);
	meta->set("created", static_cast<size_t>(created));

	return meta;
}

/*****************************************************************************/

void GameWrapper::dump() {

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		return;
	}

	auto &driver = serial::DriverMap::get(Config::get()->getString(Config::CONFIG_KEY_STATE_FORMAT));

	gameMutex.lock();

	std::string gameStatePath = Config::get()->getStatePath() +
		STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

	if (!STD_FILESYSTEM::exists(gameStatePath)) {
		STD_FILESYSTEM::create_directory(gameStatePath);
	}

	std::string gameStateSnapshotPath = gameStatePath +
		STD_FILESYSTEM::path::preferred_separator + std::to_string(
			std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now().time_since_epoch()
			).count()
		);

	STD_FILESYSTEM::create_directory(gameStateSnapshotPath);

	fstream metaFile(
		gameStateSnapshotPath + STD_FILESYSTEM::path::preferred_separator + "meta",
		std::fstream::out
	);

	metaFile << std::any_cast<std::string>(driver->serialize(serializeMeta()));
	metaFile.close();

	fstream gameFile(
		gameStateSnapshotPath + STD_FILESYSTEM::path::preferred_separator + "game",
		std::fstream::out
	);

	gameFile << std::any_cast<std::string>(driver->serialize(gamePtr->serialize()));
	gameFile.close();

	// If configured to do so, only keep the correct number of dumped games for
	// the specified game id.
	if (
		size_t maxDumpsPerGame = Config::get()->getUInt(Config::CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME);
		maxDumpsPerGame > 0
	) {

		std::set<size_t> saveSlots;
		getDumpedGameSlots(saveSlots, gameStatePath);

		while (saveSlots.size() > maxDumpsPerGame) {
			STD_FILESYSTEM::remove_all(
				gameStatePath +
				STD_FILESYSTEM::path::preferred_separator +
				std::to_string(*saveSlots.begin())
			);
			saveSlots.erase(*saveSlots.begin());
		}
	}

	gameMutex.unlock();
}

/*****************************************************************************/

void GameWrapper::destroyDump() {

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		return;
	}

	gameMutex.lock();

	std::string gameStatePath = Config::get()->getStatePath() +
		STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

	if (STD_FILESYSTEM::exists(gameStatePath)) {
		STD_FILESYSTEM::remove_all(gameStatePath);
	}

	gameMutex.unlock();
}
