#include "include/filesystem.h"
#include "include/gamecontainer.h"

#include "include/io/iostream/serverin.h"
#include "include/io/iostream/serverout.h"


// Singleton instance of GameContainer
std::unique_ptr<GameContainer> GameContainer::instance = nullptr;

/*****************************************************************************/

GameContainer::GameContainer() {

	// TODO
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

	return gameId;
}

/*****************************************************************************/

void GameContainer::destroyGame(size_t id) {

	// TODO: update player statistics here
	// Subtract number of players in game from global total

	if (games.size() > id && nullptr != games[id]) {
		playerListeners[id] = nullptr;
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
	playerListeners[gameId]->subscribe(player.get());

	// TODO: update player statistics here (global and per-game)

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

	// TODO: update player statistics here (both global and per-game)
}
