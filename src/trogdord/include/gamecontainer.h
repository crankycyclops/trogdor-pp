#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>

#include "filter/filter.h"
#include "filter/resolver.h"

#include "gamewrapper.h"
#include "inputlistener.h"

#include "exception/gamenotfound.h"
#include "exception/entity/playernotfound.h"


class GameContainer {

	protected:

		// Used to filter a set of game ids according to various criteria.
		FilterResolver<size_t> gamesResolver;

		// A combined tally of all players in all games
		size_t numPlayers = 0;

		// Singleton instance of GameContainer.
		static std::unique_ptr<GameContainer> instance;

		// All currently existing games reside here.
		std::vector<std::unique_ptr<GameWrapper>> games;

		// Each game gets its own worker thread to process player input
		std::unordered_map<size_t, std::unique_ptr<InputListener>> playerListeners;

		// Indices are used by filters to quickly return a set of games based
		// on various search criteria.
		struct {

			// Synchronizes the manipulation of indices within threads
			std::mutex mutex;

			// Set of all existing game ids
			std::set<size_t> all;

			// Indexes game ids by game name.
			std::unordered_map<std::string, std::set<size_t>> name;

			// Maps true to the set of all game ids currently running and
			// false to all game ids that exist but are currently stopped.
			std::unordered_map<bool, std::set<size_t>> running;
		} indices;

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
};


#endif
