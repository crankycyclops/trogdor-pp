#include <chrono>
#include <fstream>
#include <cstdint>

#include <trogdor/parser/parsers/xmlparser.h>
#include <trogdor/serial/serializable.h>

#include "include/config.h"
#include "include/filesystem.h"
#include "include/serial/drivermap.h"
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

GameWrapper::GameWrapper(const STD_FILESYSTEM::path &p) {

	std::string idPath = p;
	std::string idStr = p.filename();
	std::set<size_t> timestamps;

	// Find the latest timestamp and restore that version of the game
	for (const auto &subdir: STD_FILESYSTEM::directory_iterator(idPath)) {

		std::string timestamp = subdir.path().filename();

		// Skip over obviously invalid files and directories
		if (!trogdor::isValidInteger(timestamp)) {
			continue;
		}

		else if (!STD_FILESYSTEM::is_directory(subdir.path())) {
			continue;
		}

		#if SIZE_MAX == UINT64_MAX
			timestamps.insert(std::stoull(timestamp));
		#else
			timestamps.insert(std::stoul(timestamp));
		#endif
	}

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

	auto &driver = serial::DriverMap::get(Config::get()->getString(Config::CONFIG_KEY_STATE_FORMAT));

	gameMutex.lock();

	// TODO: deserialize meta and game files and initialize accordingly
	std::cout << "Stub GameWrapper::restore(): " << p << std::endl;

	gameMutex.unlock();
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

	gameStatePath += STD_FILESYSTEM::path::preferred_separator + std::to_string(
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count()
	);

	STD_FILESYSTEM::create_directory(gameStatePath);

	fstream metaFile(
		gameStatePath + STD_FILESYSTEM::path::preferred_separator + "meta",
		std::fstream::out
	);

	metaFile << std::any_cast<std::string>(driver->serialize(serializeMeta()));
	metaFile.close();

	fstream gameFile(
		gameStatePath + STD_FILESYSTEM::path::preferred_separator + "game",
		std::fstream::out
	);

	gameFile << std::any_cast<std::string>(driver->serialize(gamePtr->serialize()));
	gameFile.close();

	// TODO: if configured to keep n number of snapshots, only delete oldest
	// dirs until we reach that number. Otherwise, delete all previous timestamp
	// directories.
	gameMutex.unlock();
}
