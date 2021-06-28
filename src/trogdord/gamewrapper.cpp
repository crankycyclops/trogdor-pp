#include <chrono>
#include <fstream>
#include <cstdint>

#include <ini.h>
#include <trogdor/parser/parsers/xmlparser.h>
#include <trogdor/serial/serializable.h>

#include <trogdord/config.h>
#include <trogdord/filesystem.h>
#include <trogdord/gamecontainer.h>
#include <trogdord/serial/drivermap.h>
#include <trogdord/io/iostream/serverout.h>

#include <trogdord/exception/serverexception.h>
#include <trogdord/exception/unsupportedoperation.h>
#include <trogdord/exception/gamenotfound.h>
#include <trogdord/exception/gameslotnotfound.h>

#include <trogdord/gamewrapper.h>

// Unit tests will have to pass in a custom arument for timer's tick interval,
// while an ordinary build will want to use the default
#ifdef TIMER_CUSTOM_INTERVAL
	#define TIMER_CUSTOM_INTERVAL_ARG , TIMER_CUSTOM_INTERVAL
#else
	#define TIMER_CUSTOM_INTERVAL_ARG
#endif


std::unordered_map<std::string, std::string> GameWrapper::getDumpedGameMeta(
	const std::string &metaPath
) {

	std::unordered_map<std::string, std::string> metaData;

	if (ini_parse(metaPath.c_str(), [](
		void *data,
		const char *section,
		const char *name,
		const char *value
	) -> int {

		(*static_cast<std::unordered_map<std::string, std::string> *>(data))[name] = value;
		return 1;

	}, &metaData)) {
		throw trogdor::UndefinedException("Can't parse dumped game's meta data");
	}

	return metaData;
}

/*****************************************************************************/

void GameWrapper::writeGameMeta(std::string filename) {

	std::ofstream iniFile(filename, std::ofstream::out);

	iniFile << "[game]\n" << std::endl;
	iniFile << "name=" << name << std::endl;
	iniFile << "definition=" << definition << std::endl;
	iniFile << "created=" << static_cast<size_t>(created) << std::endl;

	iniFile.close();
}

/*****************************************************************************/

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
		TIMER_CUSTOM_INTERVAL_ARG
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

GameWrapper::GameWrapper(const STD_FILESYSTEM::path &p, std::optional<size_t> slot):
gamePtr(nullptr) {

	std::string idPath = p;
	std::string idStr = p.filename();
	std::set<size_t> slots;

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		throw UnsupportedOperation("Attempted to restore dumped game while state feature was disabled. Set state.enabled to true in trogdord.ini to use this feature.");
	}

	else if (!STD_FILESYSTEM::exists(p)) {
		throw GameNotFound();
	}

	// Find the latest dump of the game and restore it
	GameContainer::getDumpedGameSlots(slots, idPath);

	if (!slots.size()) {
		throw GameNotFound();
	}

	std::string slotStr;

	if (slot) {
		if (slots.end() != slots.find(*slot)) {
			slotStr = std::to_string(*slot);
			restoredSlot = *slot;
		} else {
			throw GameSlotNotFound();
		}
	} else {
		slotStr = std::to_string(*slots.rbegin());
		restoredSlot = *slots.rbegin();
	}

	std::string metaPath = idPath + STD_FILESYSTEM::path::preferred_separator + "meta";
	std::string formatPath = idPath + STD_FILESYSTEM::path::preferred_separator +
		slotStr + STD_FILESYSTEM::path::preferred_separator + "format";
	std::string gamePath = idPath + STD_FILESYSTEM::path::preferred_separator +
		slotStr + STD_FILESYSTEM::path::preferred_separator + "game";

	if (
		!STD_FILESYSTEM::exists(metaPath) ||
		STD_FILESYSTEM::is_directory(metaPath) ||
		!STD_FILESYSTEM::exists(gamePath) ||
		STD_FILESYSTEM::is_directory(gamePath) ||
		!STD_FILESYSTEM::exists(formatPath) ||
		STD_FILESYSTEM::is_directory(formatPath)
	) {
		throw ServerException(
			std::string("Attempted to restore an invalidly dumped game with id ") + idStr
		);
	}

	std::ifstream formatFile(formatPath);

	std::string format(
		(std::istreambuf_iterator<char>(formatFile)),
		std::istreambuf_iterator<char>()
	);

	formatFile.close();

	// Read in the game's static meta data
	std::unordered_map<std::string, std::string> metaData = getDumpedGameMeta(metaPath);

	// Deserialize game data
	auto &serialDriver = serial::DriverMap::get(format);
	std::shared_ptr<trogdor::serial::Serializable> gameData = serialDriver->deserializeFromDisk(gamePath);

	gameMutex.lock();

	name = metaData["name"];
	definition = metaData["definition"];

	#if SIZE_MAX == UINT64_MAX
		id = stoull(idStr);
		created = static_cast<time_t>(stoull(metaData["created"]));
	#else
		id = stoul(idStr);
		created = static_cast<time_t>(stoul(metaData["created"]));
	#endif

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

size_t GameWrapper::dump() {

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		return 0;
	}

	size_t dumpSlot = 0;
	std::set<size_t> slots;
	std::string driverName = Config::get()->getString(Config::CONFIG_KEY_STATE_FORMAT);
	auto &driver = serial::DriverMap::get(driverName);

	gameMutex.lock();

	std::string gameStatePath = Config::get()->getStatePath() +
		STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

	// We're dumping this game for the first time
	if (!STD_FILESYSTEM::exists(gameStatePath)) {

		STD_FILESYSTEM::create_directory(gameStatePath);

		// Save GameWrapper-specific data (this data doesn't change, so we
		// store it in the id directory instead of with each dump slot.)
		writeGameMeta(gameStatePath + STD_FILESYSTEM::path::preferred_separator + "meta");
	}

	else {

		GameContainer::getDumpedGameSlots(slots, gameStatePath);

		if (slots.size()) {
			dumpSlot = *slots.rbegin() + 1;
		}
	}

	std::string gameStateSnapshotPath = gameStatePath +
		STD_FILESYSTEM::path::preferred_separator + std::to_string(dumpSlot);
	STD_FILESYSTEM::create_directory(gameStateSnapshotPath);

	// Records when the game was dumped
	fstream timestampFile(
		gameStateSnapshotPath + STD_FILESYSTEM::path::preferred_separator + "timestamp",
		std::fstream::out
	);

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	);

	timestampFile << std::to_string(ms.count());
	timestampFile.close();

	// Records the dump slot's format
	fstream formatFile(
		gameStateSnapshotPath + STD_FILESYSTEM::path::preferred_separator + "format",
		std::fstream::out
	);

	formatFile << driverName;
	formatFile.close();

	// Serialize game data and write it to disk
	driver->writeToDisk(
		driver->serialize(gamePtr->serialize()),
		gameStateSnapshotPath + STD_FILESYSTEM::path::preferred_separator + "game"
	);

	// If configured to do so, only keep the correct number of dumped games for
	// the specified game id.
	if (
		size_t maxDumpsPerGame = Config::get()->getUInt(Config::CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME);
		maxDumpsPerGame > 0
	) {

		slots.insert(dumpSlot);

		while (slots.size() > maxDumpsPerGame) {
			STD_FILESYSTEM::remove_all(
				gameStatePath +
				STD_FILESYSTEM::path::preferred_separator +
				std::to_string(*slots.begin())
			);
			slots.erase(*slots.begin());
		}
	}

	gameMutex.unlock();
	return dumpSlot;
}

/*****************************************************************************/

void GameWrapper::destroyDump() {

	gameMutex.lock();
	GameContainer::get()->destroyDump(id);
	gameMutex.unlock();
}
