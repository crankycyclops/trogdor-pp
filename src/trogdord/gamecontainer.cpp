#include "include/filesystem.h"
#include "include/config.h"
#include "include/gamecontainer.h"


// Singleton instance of GameContainer
std::unique_ptr<GameContainer> GameContainer::instance = nullptr;

/*****************************************************************************/

GameContainer::GameContainer() {

	// TODO
}

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

size_t GameContainer::createGame(std::string name, std::string definitionPath) {

	std::string pathPrefix = TROGDORD_INSTALL_PREFIX;

	if (pathPrefix[pathPrefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
		pathPrefix += STD_FILESYSTEM::path::preferred_separator;
	}

	pathPrefix = pathPrefix + Config::get()->value<std::string>(Config::CONFIG_KEY_DEFINITIONS_PATH);

	if (pathPrefix[pathPrefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
		pathPrefix += STD_FILESYSTEM::path::preferred_separator;
	}

	definitionPath = pathPrefix + definitionPath;

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

	game->setMeta("name", name);
	games.push_back(std::move(game));
	return games.size() - 1;
}

/*****************************************************************************/

void GameContainer::destroyGame(size_t id) {

	if (games.size() > id && nullptr != games[id]) {
		games[id] = nullptr;
	}
}
