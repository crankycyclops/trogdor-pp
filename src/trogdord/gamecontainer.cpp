#include <any>

#include <trogdor/entities/player.h>

#include "include/response.h"
#include "include/gamecontainer.h"
#include "include/io/iostream/serverout.h"


// Singleton instance of GameContainer
std::unique_ptr<GameContainer> GameContainer::instance = nullptr;

/*****************************************************************************/

void GameContainer::iterateDumpedGames(
	std::string statePath,
	std::function<void(const STD_FILESYSTEM::path &)> callback,
	bool warnOnInvalid
) {

	for (const auto &subdir: STD_FILESYSTEM::directory_iterator(statePath)) {

		std::string idStr = subdir.path().filename();

		// Skip over obviously invalid files and directories
		if (!trogdor::isValidInteger(idStr)) {

			if (warnOnInvalid) {
				Config::get()->err(trogdor::Trogerr::WARNING) << idStr
					<< " in " << statePath
					<< " is not a valid game id. Skipping." << std::endl;
			}

			continue;
		}

		else if (
			!STD_FILESYSTEM::is_directory(subdir.path()) ||
			!STD_FILESYSTEM::exists(subdir.path().string() + STD_FILESYSTEM::path::preferred_separator + "meta")
		) {

			if (warnOnInvalid) {
				Config::get()->err(trogdor::Trogerr::WARNING) << idStr
					<< " in " << statePath
					<< " does not contain a dumped game. Skipping." << std::endl;
			}

			continue;
		}

		callback(subdir.path());
	}
}

/*****************************************************************************/

void GameContainer::initState() {

	if (Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {

		// Make sure we've got a valid location for storing state
		std::string statePath = Config::get()->getStatePath();

		if (0 == statePath.compare("")) {
			throw ServerException(
				std::string("If ") + Config::CONFIG_KEY_STATE_ENABLED +
				" in trogdord.ini is enabled, " + Config::CONFIG_KEY_STATE_PATH +
				" must be set to a non-empty value."
			);
		}

		if (STD_FILESYSTEM::exists(statePath)) {

			if (!STD_FILESYSTEM::is_directory(statePath)) {
				throw ServerException(
					std::string(Config::CONFIG_KEY_STATE_PATH) +
					" was set in trogdord.ini to " + statePath + ", but " + statePath +
					" is a file, not a directory."
				);
			}
		}

		else {
			throw ServerException(
				std::string(Config::CONFIG_KEY_STATE_PATH) +
				" was set in trogdord.ini to " + statePath + ", but " + statePath +
				" doesn't exist."
			);
		}

		if (!Filesystem::isDirWritable(statePath)) {
			throw ServerException(
				std::string(Config::CONFIG_KEY_STATE_PATH) +
				" must be writable in order to save state."
			);
		}

		// Any game ids that have been dumped should be reserved so that they
		// aren't assigned to newly created games, leading to possible
		// collisions when one or more dumped games are restored. This will
		// throw an exception if the directory can't be accessed.
		iterateDumpedGames(statePath, [&](const STD_FILESYSTEM::path &p) {
			games[std::stoi(p.filename())] = nullptr;
		}, true);

		if (Config::get()->getBool(Config::CONFIG_KEY_STATE_AUTORESTORE_ENABLED)) {
			restore();
		}
	}
}

/*****************************************************************************/

GameContainer::GameContainer() {

	indices.running[true] = {};
	indices.running[false] = {};

	// Filter rule that returns games that are either running or not running
	gamesResolver.addRule("is_running", [&] (Filter filter) -> std::set<size_t> {

		return indices.running[filter.getValue<bool>()];
	});

	// Filter rule that returns all games that start with a certain substring
	gamesResolver.addRule("name_starts", [&] (Filter filter) -> std::set<size_t> {

		std::set<size_t> matches;

		// std::map is an ordered data structure that allows me to start
		// iterating at the point where all words with a certain prefix begin.
		// That's why I chose to implement this index using std::map and why
		// I'm using the lower_bound method to find my first iterator position.
		for (
			auto i = indices.name.lower_bound(filter.getValue<std::string>());
			i != indices.name.end();
			i++
		) {
			if (0 == (*i).first.rfind(filter.getValue<std::string>(), 0)) {

				std::set<size_t> next;

				std::merge(
					matches.begin(),
					matches.end(),
					(*i).second.begin(),
					(*i).second.end(),
					std::inserter(next, next.begin())
				);

				matches = std::move(next);
			}

			else {
				break;
			}
		}

		return matches;
	});

	initState();
}

/*****************************************************************************/

GameContainer::~GameContainer() {

	// Optimization: this pre-step, along with its corresponding call to
	// game->shutdown() in the next loop instead of game->stop(), reduces
	// shutdown time from many seconds (in the case where there are hundreds
	// or thousands of persisted games) to almost nothing.
	for (auto &i: games) {
		if (nullptr != i.second) {
			i.second->get()->deactivate();
		}
	}

	for (auto &i: games) {
		if (nullptr != i.second) {
			i.second->get()->shutdown();
		}
	}

	games.clear();
}

/*****************************************************************************/

void GameContainer::reset() {

	// Replaces the singleton with a fresh instance
	if (instance) {
		instance = nullptr;
	}
}

/*****************************************************************************/

std::unique_ptr<GameContainer> &GameContainer::get() {

	if (!instance) {
		instance = std::unique_ptr<GameContainer>(new GameContainer());
	}

	return instance;
}

/*****************************************************************************/

const std::set<size_t> GameContainer::getGames(Filter::Union s) {

	// If there are no filters, we just return everything.
	if (!s.size()) {
		return indices.all;
	}

	// Simple optimization to avoid unnecessary filtering if the list of games
	// is currently empty.
	else if (!indices.all.size()) {
		return indices.all;
	}

	return gamesResolver.resolve(s);
}

/*****************************************************************************/

std::unique_ptr<GameWrapper> &GameContainer::getGame(size_t id) {

	// Special null unique_ptr that we can return a reference to when a game
	// of the specified id doesn't exist.
	static std::unique_ptr<GameWrapper> nullGame = nullptr;

	const auto &game = games.find(id);
	return games.end() != game ? game->second : nullGame;
}

/*****************************************************************************/

size_t GameContainer::createGame(
	std::string definitionPath,
	std::string name,
	std::unordered_map<std::string, std::string> meta
) {

	size_t gameId = !games.size() ? 0 : games.rbegin()->first + 1;
	games[gameId] = std::make_unique<GameWrapper>(gameId, definitionPath, name, meta);

	// Update the running index whenever the game is started
	games[gameId]->get()->addCallback("start",
	std::make_shared<trogdor::Game::GameCallback>([&, gameId](std::any data) -> bool {

		indexStarted(gameId, true);
		return false;
	}));

	// Update the running index whenever the game is stopped
	games[gameId]->get()->addCallback("stop",
	std::make_shared<trogdor::Game::GameCallback>([&, gameId](std::any data) -> bool {

		indexStarted(gameId, false);
		return false;
	}));

	// Make sure players drop their inventory (including droppable objects)
	// when they're removed from the game so that no items are lost
	games[gameId]->get()->addCallback("removePlayer",
	std::make_shared<trogdor::Game::GameCallback>([&](std::any player) -> bool {

		auto invObjects = std::any_cast<
			std::shared_ptr<trogdor::entity::Player>
		>(player)->getInventoryObjects();

		for (auto &object: invObjects) {
			std::any_cast<
				std::shared_ptr<trogdor::entity::Player>
			>(player)->drop(object, false, false);
		}

		return false;
	}));

	indexNewGame(gameId);
	return gameId;
}

/*****************************************************************************/

void GameContainer::destroyGame(size_t id, bool destroyDump) {

	auto game = games.find(id);

	if (games.end() != game && nullptr != game->second) {

		if (destroyDump) {
			games[id]->destroyDump();
		}

		numPlayers -= games[id]->getNumPlayers();
		clearIndices(id);
		games[id] = nullptr;
	}
}

/*****************************************************************************/

void GameContainer::startGame(size_t id) {

	auto game = games.find(id);

	if (games.end() != game && nullptr != game->second) {
		games[id]->get()->start();
	}
}

/*****************************************************************************/

void GameContainer::stopGame(size_t id) {

	auto game = games.find(id);

	if (games.end() != game && nullptr != game->second) {
		games[id]->get()->stop();
	}
}

/*****************************************************************************/

std::unordered_map<std::string, std::string> GameContainer::getMetaAll(size_t gameId) {

	std::unique_ptr<GameWrapper> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	return std::unordered_map<std::string, std::string>(game->get()->getMetaAll());
}

/*****************************************************************************/

std::string GameContainer::getMeta(size_t gameId, std::string key) {

	std::unique_ptr<GameWrapper> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	return game->get()->getMeta(key);
}

/*****************************************************************************/

void GameContainer::setMeta(size_t gameId, std::string key, std::string value) {

	std::unique_ptr<GameWrapper> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	game->get()->setMeta(key, value);
}

/*****************************************************************************/

trogdor::entity::Player *GameContainer::createPlayer(size_t gameId, std::string playerName) {

	std::unique_ptr<GameWrapper> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	std::shared_ptr<trogdor::entity::Player> player = game->get()->createPlayer(
		playerName,
		std::make_unique<ServerOut>(gameId),
		Config::get()->err().copy()
	);

	static_cast<ServerOut *>(&(player->out()))->setEntity(player.get());
	game->get()->insertPlayer(player);

	numPlayers++;
	return player.get();
}

/*****************************************************************************/

void GameContainer::removePlayer(size_t gameId, std::string playerName, std::string message) {

	std::unique_ptr<GameWrapper> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	std::shared_ptr<trogdor::entity::Player> pPtr = game->get()->getPlayer(playerName);

	if (!pPtr) {
		throw PlayerNotFound();
	}

	game->get()->removePlayer(playerName, message);
	numPlayers--;
}

/*****************************************************************************/

void GameContainer::dump() {

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		return;
	}

	size_t numGames = 0;

	for (auto &game: games) {
		if (game.second) {
			game.second->dump();
			numGames++;
		}
	}

	Config::get()->err(trogdor::Trogerr::INFO) << "Dumped " << numGames <<
		" games to disk." << std::endl;
}

/*****************************************************************************/

int GameContainer::restore() {

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		return Response::STATUS_UNSUPPORTED;
	}

	size_t numGames = 0;
	size_t numSuccessful = 0;

	// I don't need to check this because I already did that in initStatePath().
	std::string statePath = Config::get()->getStatePath();

	iterateDumpedGames(statePath, [&](const STD_FILESYSTEM::path &p) {

		std::string idStr = p.filename();

		try {

			numGames++;

			size_t gameId = std::stoi(idStr);

			games[gameId] = std::make_unique<GameWrapper>(p);
			indexNewGame(gameId);
			numSuccessful++;

			Config::get()->err(trogdor::Trogerr::INFO) << "Restored game id "
				<< idStr << '.' << std::endl;
		}

		catch (const std::exception &e) {
			Config::get()->err(trogdor::Trogerr::ERROR) << "Failed to restore game id "
				<< idStr << ": " << e.what() << std::endl;
		}
	});

	return numGames == numSuccessful ? Response::STATUS_SUCCESS : Response::STATUS_PARTIAL_CONTENT;
}

/*****************************************************************************/

bool GameContainer::isDumpedGameId(size_t id) {

	if (Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {

		std::string dumpPath = Config::get()->getStatePath() +
			STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

		if (STD_FILESYSTEM::exists(dumpPath) && STD_FILESYSTEM::is_directory(dumpPath)) {
			return true;
		}
	}

	return false;
}

/*****************************************************************************/

std::vector<size_t> GameContainer::getDumpedGameIds() {

	std::vector<size_t> ids;

	iterateDumpedGames(Config::get()->getStatePath(), [&](const STD_FILESYSTEM::path &p) {

		std::string idStr = p.filename();

		try {
			ids.push_back(std::stoi(idStr));
		} catch (const std::exception &e) {}
	});

	return ids;
}
