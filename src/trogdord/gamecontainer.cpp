#include "include/filesystem.h"
#include "include/gamecontainer.h"

#include "include/io/iostream/streamout.h"
#include <trogdor/iostream/nullin.h>


// Singleton instance of GameContainer
std::unique_ptr<GameContainer> GameContainer::instance = nullptr;

/*****************************************************************************/

PlayerInputListener::~PlayerInputListener() {

	stop();
}

/*****************************************************************************/

void PlayerInputListener::subscribe(trogdor::entity::Player *pPtr) {

	PlayerFuture pf;

	pf.playerPtr = pPtr;
	processed[pPtr->getName()] = std::move(pf);
}

/*****************************************************************************/

void PlayerInputListener::unsubscribe(std::string playerName) {

	// Do the actual removal in the worker thread after we're sure we're not
	// waiting on anymore commands.
	if (processed.end() != processed.find(playerName)) {
		processed[playerName].playerPtr = nullptr;
	}
}

/*****************************************************************************/

void PlayerInputListener::start() {

	if (!on) {

		on = true;

		// Initialize the worker thread with a list of players whose commands
		// need to be listened for and processed.
		for (const auto &player: gamePtr->getPlayers()) {
			subscribe(static_cast<trogdor::entity::Player *>(player.second.get()));
		}

		// Worker thread that will listen for and process player commands.
		worker = std::thread([&]() {

			// How long to sleep the thread before polling again
			static std::chrono::milliseconds sleepTime(
				PlayerInputListener::THREAD_SLEEP_MS
			);

			while (on) {

				for (auto &next: processed) {

					// If a command for this player has already been
					// processed, or if this is the first time the thread
					// is executed, process the next command.
					if (!next.second.initialized || next.second.isReady()) {

						if (next.second.playerPtr) {

							next.second.future = std::async(
								std::launch::async,
								[&](trogdor::entity::Player *pPtr) -> bool {
									gamePtr->processCommand(pPtr);
									return true;
								},
								next.second.playerPtr
							);

							next.second.initialized = true;
						}

						// The player was removed from the game, so stop
						// listening for their commands.
						else {
							// TODO: send null command
							next.second.future.wait();
							processed.erase(next.first);
						}
					}
				}

				std::this_thread::sleep_for(sleepTime);
			}
		});
	}
}

/*****************************************************************************/

void PlayerInputListener::stop() {

	if (on) {
		on = false;
		worker.join();
	}
}

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
		games.back()->shutdown();
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

	game->setMeta("name", name);
	games.push_back(std::move(game));

	size_t gameId = games.size() - 1;

	playerListeners.insert(
		std::make_pair(
			gameId,
			std::make_unique<PlayerInputListener>(games[gameId].get())
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

trogdor::entity::Player *GameContainer::createPlayer(size_t gameId, std::string playerName) {

	std::unique_ptr<trogdor::Game> &game = getGame(gameId);

	if (!game) {
		throw GameNotFound();
	}

	// TODO: use StreamIn instead of NullIn
	std::shared_ptr<trogdor::entity::Player> player = game->createPlayer(
		playerName,
		std::make_unique<StreamOut>(gameId),
		std::make_unique<trogdor::NullIn>(),
		Config::get()->err().copy()
	);

	static_cast<StreamOut *>(&(player->out()))->setEntity(player.get());

	game->insertPlayer(player);
	playerListeners[gameId]->subscribe(player.get());

	return player.get();
}
