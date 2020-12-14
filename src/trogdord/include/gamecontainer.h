#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>

#include "filesystem.h"
#include "filter/filter.h"
#include "filter/resolver.h"

#include "gamewrapper.h"

#include "exception/gamenotfound.h"
#include "exception/entity/playernotfound.h"


class GameContainer {

	protected:

		// Singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> instance;

		// Used to filter a set of game ids according to various criteria.
		FilterResolver<size_t> gamesResolver;

		// A combined tally of all players in all games.
		size_t numPlayers = 0;

		// Maps game ids to games. std::map ensures sorting by id.
		std::map<size_t, std::unique_ptr<GameWrapper>> games;

		// Indices are used by filters to quickly return a set of games based
		// on various search criteria.
		struct {

			// Synchronizes the manipulation of indices within threads
			std::mutex mutex;

			// Set of all existing game ids
			std::set<size_t> all;

			// Indexes game ids by game name. I chose the map instead of the
			// unordered_map because it allows for efficient searching based
			// on string prefixes.
			std::map<std::string, std::set<size_t>> name;

			// Maps true to the set of all game ids currently running and
			// false to all game ids that exist but are currently stopped.
			std::unordered_map<bool, std::set<size_t>> running;
		} indices;

		// Protected constructor, making get() the only way to return an
		// instance.
		GameContainer();
		GameContainer(const GameContainer &) = delete;

		// Sets up state management so that we can save and restore games.
		void initState();

		// Indexes a game id according to whether or not it's started
		inline void indexStarted(size_t gameId, bool started, bool lock = true) {

			if (lock) {
				indices.mutex.lock();
			}

			indices.running[!started].erase(gameId);
			indices.running[started].insert(gameId);

			if (lock) {
				indices.mutex.unlock();
			}
		}

		// Indexes a new game right after it's been created.
		inline void indexNewGame(size_t gameId) {

			indices.mutex.lock();

			// Note that the game is always initialized in a stopped state.
			indexStarted(gameId, games[gameId]->get()->inProgress(), false);
			indices.all.insert(gameId);

			if (indices.name.end() == indices.name.find(games[gameId]->getName())) {
				indices.name[games[gameId]->getName()] = {};
			}

			indices.name[games[gameId]->getName()].insert(gameId);

			indices.mutex.unlock();
		}

		// Removes all indices for the given game.
		inline void clearIndices(size_t gameId) {

			indices.mutex.lock();

			indices.running[true].erase(gameId);
			indices.running[false].erase(gameId);
			indices.all.erase(gameId);

			indices.name[games[gameId]->getName()].erase(gameId);

			if (!indices.name[games[gameId]->getName()].size()) {
				indices.name.erase(games[gameId]->getName());
			}

			indices.mutex.unlock();
		}

		// Applies the specified operation to each dumped game.
		static void iterateDumpedGames(
			std::string statePath,
			std::function<void(const STD_FILESYSTEM::path &)> callback,
			bool warnOnInvalid = false
		);

	public:

		// Ensures all games are properly shutdown before the server goes down
		~GameContainer();

		// Resets the singleton instance of GameContainer to a nullptr,
		// ensuring that the destructor is called and that a fresh instance
		// will be returned on the next call to get().
		static void reset();

		// Returns singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> &get();

		// Returns the current number of existing games.
		inline size_t size() {return indices.all.size();}

		// Returns an iterable set of game ids matching certain criteria (or
		// all game ids if no filters are specified.)
		const std::set<size_t> getGames(Filter::Union s = {});

		// Returns the game referenced by the given id (returns nullptr if
		// it doesn't exist.)
		std::unique_ptr<GameWrapper> &getGame(size_t id);

		// Creates a new game and returns its id. Takes as input a name that
		// the game should be identified by, a relative path to the definition
		// that should be used to create it, an an optional map of key => value
		// pairs representing meta data that should be inserted into the game.
		// Throws an instance of ServerException or trogdor::Exception if the
		// game cannot be created.
		size_t createGame(
			std::string definitionPath,
			std::string name,
			std::unordered_map<std::string, std::string> meta = {}
		);

		// Returns the current combined total of all players in all games
		inline const size_t getNumPlayers() const {return numPlayers;}

		// Destroys the game referenced by the given id (does nothing if the
		// game doesn't exist.)
		void destroyGame(size_t id);

		// Starts the game referenced by the given id.
		void startGame(size_t id);

		// Stops the game referenced by the given id.
		void stopGame(size_t id);

		// Returns all set meta key, value pairs in the game.
		std::unordered_map<std::string, std::string> getMetaAll(size_t gameId);

		// Returns the specified meta value from the game.
		std::string getMeta(size_t gameId, std::string key);

		// Sets the specified meta value in the game.
		void setMeta(size_t gameId, std::string key, std::string value);

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

		// Dumps the server's current state, including all existing games.
		// Does nothing if the state feature was disabled.
		void dump();

		// Restores the server's state, including all saved games. This will
		// not overwrite any newly created games, and since initState()
		// reserves previously dumped game ids, id collisions shouldn't occur.
		// Does nothing if the state feature was disabled. Returns a status
		// code, which can be sent back if call was part of a request.
		int restore();
};


#endif
