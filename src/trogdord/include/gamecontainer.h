#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <memory>
#include <thread>
#include <future>
#include <vector>
#include <queue>
#include <unordered_map>

#include <trogdor/game.h>
#include <trogdor/parser/parsers/xmlparser.h>

#include "exception/gamenotfound.h"


struct PlayerFuture {

	// True once the future has been used to process a player command for the
	// first time.
	bool initialized = false;

	// Pointer back to the player who issued the command. Setting this to
	// nullptr indicates that the player has been removed from the game and
	// that we should stop listening for their commands.
	trogdor::entity::Player *playerPtr;

	// This will contain a value after trogdor::Game::processCommand() returns.
	std::future<bool> future;

	// Returns true if the future is ready and false if not.
	inline bool isReady() {

		return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	// Comparison operator (required for use of std::remove in case where we
	// need to stop listening for the player's input.)
	inline bool operator==(const PlayerFuture &other) {

		return playerPtr == other.playerPtr;
	}
};

/*****************************************************************************/

class PlayerInputListener {

	private:

		// Number of milliseconds to sleep player input threads after one
		// round of checking for player input.
		static const unsigned int THREAD_SLEEP_MS = 10;

		// Setting this to false after the input listener has been started
		// will stop the worker thread.
		bool on = false;

		// The game the input listener processes player commands for
		trogdor::Game *gamePtr;

		// Each game gets a worker thread that listens for player commands.
		std::thread worker;

		// A future for each player in the game that will contain a value once
		// trogdor::Game::processCommand() returns, indicating that a command
		// has been executed and that we should listen for another command
		// from the same player.
		std::unordered_map<std::string, PlayerFuture> processed;

	public:

		// Constructor
		inline PlayerInputListener(trogdor::Game *gPtr): gamePtr(gPtr) {}
		//PlayerInputListener() = delete;
		//PlayerInputListener(const PlayerInputListener &) = delete;

		// Destructor
		~PlayerInputListener();

		// Start listening for input from the given player.
		void subscribe(trogdor::entity::Player *pPtr);

		// Stop listening for input from the given player.
		void unsubscribe(std::string playerName);

		// Stop listening for input from the given player.
		inline void unsubscribe(trogdor::entity::Player *pPtr) {

			unsubscribe(pPtr->getName());
		}

		// Start the player input listener thread
		void start();

		// Stop the player input listener thread
		void stop();
};

/*****************************************************************************/

class GameContainer {

	protected:

		// Singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> instance;

		// All currently existing games reside here.
		std::vector<std::unique_ptr<trogdor::Game>> games;

		// Each game gets its own worker thread to process player input
		std::unordered_map<size_t, std::unique_ptr<PlayerInputListener>> playerListeners;

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
};


#endif
