#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <vector>
#include <queue>
#include <memory>

#include <trogdor/game.h>


class GameContainer {

	protected:

		// Singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> instance;

		// Indices in games that have been reclaimed (had their Game pointers
		// set to nullptr.)
		std::queue<size_t> recycledIds;

		// All currently existing games reside here.
		std::vector<std::unique_ptr<trogdor::Game>> games;

		// Protected constructor, making get() the only way to return an
		// instance.
		GameContainer();
		GameContainer(const GameContainer &) = delete;

	public:

		// Ensures all games are properly shutdown before the server goes down
		~GameContainer();

		// Returns singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> &get();

		// Returns the current number of existing games.
		inline size_t size() {return games.size();}

		// Returns the game referenced by the given id (returns nullptr if
		// it doesn't exist.)
		std::unique_ptr<trogdor::Game> &getGame(size_t id);

		// Creates a new game and returns its id.
		size_t createGame();

		// Destroys the game referenced by the given id (does nothing if the
		// game doesn't exist.)
		void destroyGame(size_t id);
};


#endif
