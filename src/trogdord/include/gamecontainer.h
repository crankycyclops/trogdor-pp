#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <future>

#include <trogdor/game.h>
#include <trogdor/parser/parsers/xmlparser.h>


class PlayerInputListener {

	private:

		// Setting this to false after the input listener has been started
		// will stop the worker thread.
		bool on = false;

		// The game the input listener processes player commands for
		trogdor::Game *gamePtr;

		// Each game gets a worker thread that listens for player commands.
		std::thread worker;

		// A future for each player in the game that will contain a value once
		// trogdor::Game::processCommand() returns, indicating that a command
		// has been executed and that we should listen for another from the same
		// player.
		std::vector<std::future<bool>> processed;

	public:

		// Number of milliseconds to sleep player input threads after one
		// round of checking for player input.
		static const unsigned int THREAD_SLEEP_MS = 10;

		// Constructor
		inline PlayerInputListener(trogdor::Game *gPtr): gamePtr(gPtr) {}
		//PlayerInputListener() = delete;
		//PlayerInputListener(const PlayerInputListener &) = delete;

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
};


#endif
