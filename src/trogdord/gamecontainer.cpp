#include "include/gamecontainer.h"


// Singleton instance of GameContainer
std::unique_ptr<GameContainer> GameContainer::instance = nullptr;

/*****************************************************************************/

GameContainer::~GameContainer() {

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
		games.pop_back();
	}
}

/*****************************************************************************/

GameContainer::GameContainer() {

	// TODO
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

size_t GameContainer::createGame() {

	// TODO
	return 0;
}

/*****************************************************************************/

void GameContainer::destroyGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		games[id] = nullptr;
		recycledIds.push(id);
	}
}
