#include "include/filesystem.h"
#include "include/gamecontainer.h"

#include "include/io/iostream/serverin.h"
#include "include/io/iostream/serverout.h"


// Key names for various game meta data
const char *GameContainer::META_KEY_NAME = "_trogdord_name";

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
			game->deactivate();
		}
	}

	// Destructor for trogdor::Game will be called once the unique_ptr falls
	// out of scope.
	while (!games.empty()) {

		if (nullptr != games.back()) {
			games.back()->shutdown();
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

std::unique_ptr<trogdor::Game> &GameContainer::getGame(size_t id) {

	// Special null unique_ptr that we can return a reference to when a game
	// of the specified id doesn't exist.
	static std::unique_ptr<trogdor::Game> nullGame = nullptr;

	return id < games.size() ? games[id] : nullGame;
}

/*****************************************************************************/

size_t GameContainer::createGame(
	std::string definitionPath,
	std::string name,
	std::unordered_map<std::string, std::string> meta
) {

	definitionPath = Filesystem::getFullDefinitionsPath(definitionPath);

	// TODO: will need better and more specific error logging than just a
	// simple copy of the global server error logger
	std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
		Config::get()->err().copy()
	);

	std::unique_ptr<trogdor::XMLParser> parser = std::make_unique<trogdor::XMLParser>(
		game->makeInstantiator(), game->getVocabulary()
	);

	if (!game->initialize(parser.get(), definitionPath)) {
		throw ServerException("failed to initialize game");
	}

	game->setMeta(META_KEY_NAME, name);

	// If any custom meta data was specified, set it
	for (auto &pair: meta) {
		game->setMeta(pair.first, pair.second);
	}

	games.push_back(std::move(game));

	size_t gameId = games.size() - 1;

	playerListeners.insert(
		std::make_pair(
			gameId,
			std::make_unique<InputListener>(gameId, games[gameId].get())
		)
	);

	return gameId;
}

/*****************************************************************************/

void GameContainer::destroyGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		playerListeners[id] = nullptr;
		games[id] = nullptr;
	}
}

/*****************************************************************************/

void GameContainer::startGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		games[id]->start();
		playerListeners[id]->start();
	}
}

/*****************************************************************************/

void GameContainer::stopGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		games[id]->stop();
		playerListeners[id]->stop();
	}
}

/*****************************************************************************/

std::unordered_map<std::string, std::string> GameContainer::getMetaAll(size_t gameId) {

	std::unique_ptr<trogdor::Game> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	return std::unordered_map<std::string, std::string>(game->getMetaAll());
}

/*****************************************************************************/

std::string GameContainer::getMeta(size_t gameId, std::string key) {

	std::unique_ptr<trogdor::Game> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	return game->getMeta(key);
}

/*****************************************************************************/

void GameContainer::setMeta(size_t gameId, std::string key, std::string value) {

	std::unique_ptr<trogdor::Game> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	game->setMeta(key, value);
}

/*****************************************************************************/

trogdor::entity::Player *GameContainer::createPlayer(size_t gameId, std::string playerName) {

	std::unique_ptr<trogdor::Game> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	std::shared_ptr<trogdor::entity::Player> player = game->createPlayer(
		playerName,
		std::make_unique<ServerOut>(gameId),
		std::make_unique<ServerIn>(gameId),
		Config::get()->err().copy()
	);

	static_cast<ServerIn *>(&(player->in()))->setEntity(player.get());
	static_cast<ServerOut *>(&(player->out()))->setEntity(player.get());

	game->insertPlayer(player);
	playerListeners[gameId]->subscribe(player.get());

	return player.get();
}

/*****************************************************************************/

void GameContainer::removePlayer(size_t gameId, std::string playerName, std::string message) {

	std::unique_ptr<trogdor::Game> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	trogdor::entity::Player *pPtr = game->getPlayer(playerName);

	if (!pPtr) {
		throw PlayerNotFound();
	}

	// If we're already processing a player's input, we'll have to remove them
	// from the game after unblocking their input stream.
	if (static_cast<ServerIn &>(pPtr->in()).isBlocked()) {
		playerListeners[gameId]->unsubscribe(pPtr, [&game, playerName, message] {
			game->removePlayer(playerName, message);
		});
	}

	else {
		playerListeners[gameId]->unsubscribe(pPtr);
		game->removePlayer(playerName, message);
	}
}
