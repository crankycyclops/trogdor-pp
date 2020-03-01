#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <unordered_map>

#include <trogdor/game.h>
#include <trogdor/parser/parsers/xmlparser.h>

#include "inputlistener.h"

#include "exception/gamenotfound.h"
#include "exception/entity/playernotfound.h"


class GameContainer {

	protected:

		// Singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> instance;

		// All currently existing games reside here.
		std::vector<std::unique_ptr<trogdor::Game>> games;

		// Each game gets its own worker thread to process player input
		std::unordered_map<size_t, std::unique_ptr<InputListener>> playerListeners;

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

		// Creates a player and inserts it into the game. Might throw an
		// instance of trogdor::Exception if player creation is unsuccessful
		// or GameNotFound if the specified game ID doesn't correspond to an
		// existing game.
		trogdor::entity::Player *createPlayer(size_t gameId, std::string playerName);

		// Removes a player from the game. Throws GameNotFound if the specified
		// game ID doesn't correspond to an existing game or PlayerNotFound if
		// the specified player doesn't exist in the given game. If message is
		// set to a non-empty string, that message will be sent to the player's
		// notifications channel before they're removed.
		void removePlayer(size_t gameId, std::string playerName, std::string message = "");
};


#endif
