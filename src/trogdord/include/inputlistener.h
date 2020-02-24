#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <future>

#include <trogdor/game.h>
#include <trogdor/entities/player.h>


struct PlayerFuture {

	// True once the future has been used to process a player command for the
	// first time.
	bool initialized = false;

	// Pointer back to the player who issued the command. Setting this to
	// nullptr indicates that the player has been removed from the game and
	// that we should stop listening for their commands.
	trogdor::entity::Player *playerPtr;

	// Name of the player who issued the command. Necessary to send null input
	// to the player's input stream after playerPtr has been set to nullptr by
	// InputListener's unsubscribe() method.
	std::string playerName;

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

class InputListener {

	private:

		// Number of milliseconds to sleep player input threads after one
		// round of checking for player input.
		static const unsigned int THREAD_SLEEP_MS = 10;

		// Setting this to false after the input listener has been started
		// will stop the worker thread.
		bool on = false;

		// I need the game ID to use the input buffer.
		size_t gameId;

		// The game the input listener processes player commands for
		trogdor::Game *gamePtr;

		// Each game gets a worker thread that listens for player commands.
		std::thread worker;

		// A future for each player in the game that will contain a value once
		// trogdor::Game::processCommand() returns, indicating that a command
		// has been executed and that we should listen for another command
		// from the same player.
		std::unordered_map<std::string, PlayerFuture> processed;

		// Synchronizes access to processed (defined above)
		std::mutex processedMutex;

		// Start listening for input from the given player. Allows mutex
		// locking to be turned on and off.
		void _subscribe(trogdor::entity::Player *pPtr, bool lock = true);

		// Stop listening for input from the given player. Allows mutex
		// locking to be turned on and off.
		void _unsubscribe(std::string playerName, bool lock = true);

		void reapUnsubscribed();

	public:

		// Constructor
		inline InputListener(size_t gId, trogdor::Game *gPtr): gameId(gId), gamePtr(gPtr) {}
		InputListener() = delete;
		InputListener(const InputListener &) = delete;

		// Destructor
		~InputListener();

		// Start listening for input from the given player. This public access
		// to _subscribe forces mutex locking.
		inline void subscribe(trogdor::entity::Player *pPtr) {

			_subscribe(pPtr);
		}

		// Stop listening for input from the given player. This public access
		// to _unsubscribe forces mutex locking.
		inline void unsubscribe(std::string playerName) {

			_unsubscribe(playerName);
		}

		// Stop listening for input from the given player.
		inline void unsubscribe(trogdor::entity::Player *pPtr) {

			_unsubscribe(pPtr->getName());
		}

		// Start the player input listener thread
		void start();

		// Stop the player input listener thread
		void stop();
};


#endif
