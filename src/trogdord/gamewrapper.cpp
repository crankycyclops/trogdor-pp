#include <chrono>
#include <fstream>

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

	gameMutex.lock();

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		return;
	}

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

	std::shared_ptr<trogdor::serial::Serializable> meta = serializeMeta();
	std::shared_ptr<trogdor::serial::Serializable> game = gamePtr->serialize();

	auto &driver = serial::DriverMap::get(Config::get()->getString(Config::CONFIG_KEY_STATE_FORMAT));

	fstream metaFile(
		gameStatePath + STD_FILESYSTEM::path::preferred_separator + "meta",
		std::fstream::out
	);

	fstream gameFile(
		gameStatePath + STD_FILESYSTEM::path::preferred_separator + "game",
		std::fstream::out
	);

	metaFile << std::any_cast<std::string>(driver->serialize(meta));
	metaFile.close();

	gameFile << std::any_cast<std::string>(driver->serialize(game));
	gameFile.close();

	// TODO: if configured to keep n number of snapshots, only delete oldest
	// dirs until we reach that number. Otherwise, delete all previous timestamp
	// directories.
	gameMutex.unlock();
}