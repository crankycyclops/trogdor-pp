#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <vector>
#include <queue>
#include <memory>

#include <trogdor/game.h>
#include <trogdor/parser/parsers/xmlparser.h>


class GameContainer {

	protected:

		// Singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> instance;

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

		// Returns a read-only reference to games so that we can iterate over
		// it from the outside.
		inline const std::vector<std::unique_ptr<trogdor::Game>> &getGames() {return games;}

		// Returns the game referenced by the given id (returns nullptr if
		// it doesn't exist.)
		std::unique_ptr<trogdor::Game> &getGame(size_t id);

		// Creates a new game and returns its id. Takes as input a name that
		// the game should be identified by and a relative path to the
		// definition that should be used to create it. Throws an instance of
		// ServerException or trogdor::Exception if the game cannot be created.
		size_t createGame(std::string name, std::string definitionPath);

		// Destroys the game referenced by the given id (does nothing if the
		// game doesn't exist.)
		void destroyGame(size_t id);

		// Starts the game referenced by the given id.
		void startGame(size_t id);

		// Stops the game referenced by the given id.
		void stopGame(size_t id);
};


#endif
