#include "include/filesystem.h"
#include "include/gamecontainer.h"

#include "include/io/iostream/serverin.h"
#include "include/io/iostream/serverout.h"


// Singleton instance of GameContainer
std::unique_ptr<GameContainer> GameContainer::instance = nullptr;

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

		// TODO: Iterating through all game names probably isn't the best
		// approach. I'll do some more research into a better solution and
		// re-write the logic and index data structure.
		for (auto const &game: indices.name) {

			if (game.first.rfind(filter.getValue<std::string>(), 0) == 0) {

				std::set<size_t> next;

				std::merge(
					matches.begin(),
					matches.end(),
					game.second.begin(),
					game.second.end(),
					std::inserter(next, next.begin())
				);

				matches = std::move(next);
			}
		}

		return matches;
	});
}

/*****************************************************************************/

GameContainer::~GameContainer() {

	// Stop listening for player input on all currently running games and free
	// the listener's memory.
	playerListeners.clear();

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

	playerListeners.insert(
		std::make_pair(
			gameId,
			// The double get resolves like so: GameWrapper -> unique_ptr<trogdord::Game> -> trogdord::Game *
			std::make_unique<InputListener>(gameId, games[gameId]->get().get())
		)
	);

	// Update the running index whenever the game is started
	games[gameId]->get()->addCallback("start", std::make_shared<std::function<void()>>([&, gameId] {

		indices.mutex.lock();

		indices.running[false].erase(gameId);
		indices.running[true].insert(gameId);

		indices.mutex.unlock();
	}));

	// Update the running index whenever the game is stopped
	games[gameId]->get()->addCallback("stop", std::make_shared<std::function<void()>>([&, gameId] {

		indices.mutex.lock();

		indices.running[true].erase(gameId);
		indices.running[false].insert(gameId);

		indices.mutex.unlock();
	}));

	// Note that the game is always initialized in a stopped state.
	indices.mutex.lock();

	indices.all.insert(gameId);
	indices.running[false].insert(gameId);

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
		playerListeners[id] = nullptr;

		indices.mutex.lock();

		indices.running[true].erase(id);
		indices.running[false].erase(id);
		indices.all.erase(id);

		indices.name[games[id]->getName()].erase(id);

		if (!indices.name[games[id]->getName()].size()) {
			indices.name.erase(games[id]->getName());
		}

		indices.mutex.unlock();

		games[id] = nullptr;
	}
}

/*****************************************************************************/

void GameContainer::startGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		games[id]->get()->start();
		playerListeners[id]->start();
	}
}

/*****************************************************************************/

void GameContainer::stopGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		games[id]->get()->stop();
		playerListeners[id]->stop();
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
		std::make_unique<ServerIn>(gameId),
		Config::get()->err().copy()
	);

	static_cast<ServerIn *>(&(player->in()))->setEntity(player.get());
	static_cast<ServerOut *>(&(player->out()))->setEntity(player.get());

	game->get()->insertPlayer(player);

	if (game->get()->inProgress()) {
		playerListeners[gameId]->subscribe(player.get());
	}

	numPlayers++;
	return player.get();
}

/*****************************************************************************/

void GameContainer::removePlayer(size_t gameId, std::string playerName, std::string message) {

	std::unique_ptr<GameWrapper> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	trogdor::entity::Player *pPtr = game->get()->getPlayer(playerName);

	if (!pPtr) {
		throw PlayerNotFound();
	}

	// If we're already processing a player's input, we'll have to remove them
	// from the game after unblocking their input stream.
	if (static_cast<ServerIn &>(pPtr->in()).isBlocked()) {
		playerListeners[gameId]->unsubscribe(pPtr, [&game, playerName, message] {
			game->get()->removePlayer(playerName, message);
		});
	}

	else {
		playerListeners[gameId]->unsubscribe(pPtr);
		game->get()->removePlayer(playerName, message);
	}

	numPlayers--;
}
