#include <any>

#include <trogdor/entities/player.h>

#include "include/filesystem.h"
#include "include/gamecontainer.h"

#include "include/io/iostream/serverout.h"


// Singleton instance of GameContainer
std::unique_ptr<GameContainer> GameContainer::instance = nullptr;

/*****************************************************************************/

void GameContainer::initState() {

	if (Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {

		// Make sure we've got a valid location for storing state
		std::string statePath = Config::get()->getString(Config::CONFIG_KEY_STATE_PATH);
		statePath = trogdor::trim(statePath);

		if (0 == statePath.compare("")) {
			throw ServerException(
				std::string("If ") + Config::CONFIG_KEY_STATE_ENABLED +
				" in trogdord.ini is enabled, " + Config::CONFIG_KEY_STATE_PATH +
				" must be set to a non-empty value."
			);
		}

		statePath = Filesystem::getAbsolutePath(statePath);

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

		// TODO: at this point, we know the directory exists and we can attempt
		// to iterate over it (will have to catch errors -- ex: permissions)
		Config::get()->err(trogdor::Trogerr::INFO) << "TODO: finish setting up state management" << std::endl;
		// TODO: if auto-restore is enabled, call restore()
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
	for (auto &game: games) {
		if (nullptr != game) {
			game->get()->deactivate();
		}
	}

	// Destructor for trogdor::Game will be called once the unique_ptr falls
	// out of scope.
	while (!games.empty()) {

		if (nullptr != games.back()) {
			games.back()->get()->shutdown();
		}

		games.pop_back();
	}
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

	return id < games.size() ? games[id] : nullGame;
}

/*****************************************************************************/

size_t GameContainer::createGame(
	std::string definitionPath,
	std::string name,
	std::unordered_map<std::string, std::string> meta
) {

	games.push_back(std::make_unique<GameWrapper>(definitionPath, name, meta));

	size_t gameId = games.size() - 1;

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

	indices.mutex.lock();

	// Note that the game is always initialized in a stopped state.
	indexStarted(gameId, false, false);
	indices.all.insert(gameId);

	if (indices.name.end() == indices.name.find(name)) {
		indices.name[name] = {};
	}

	indices.name[name].insert(gameId);

	indices.mutex.unlock();
	return gameId;
}

/*****************************************************************************/

void GameContainer::destroyGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		numPlayers -= games[id]->getNumPlayers();
		clearIndices(id);
		games[id] = nullptr;
	}
}

/*****************************************************************************/

void GameContainer::startGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		games[id]->get()->start();
	}
}

/*****************************************************************************/

void GameContainer::stopGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
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

void GameContainer::restore() {

	// TODO: recover all games
}
