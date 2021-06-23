#ifndef GAME_WRAPPER_H
#define GAME_WRAPPER_H

#include <memory>
#include <ctime>
#include <utility>
#include <trogdor/game.h>

#include "filesystem.h"


class GameWrapper {

	private:

		// Lock on this whenever we modify GameWrapper, the contained game, or
		// need to do any atomic operation that requires the state of the
		// GameWrapper instance to remain unchanged until it's complete.
		std::mutex gameMutex;

		// The game's id
		size_t id;

		// The game's name
		std::string name;

		// The game's definition filename (relative path)
		std::string definition;

		// When the game was created
		std::time_t created;

		// The actual underlying game object
		std::unique_ptr<trogdor::Game> gamePtr;

		// If set, this is the dump slot that was restored when the instance
		// of GameWrapper was instantiated
		std::optional<size_t> restoredSlot;

		// Save all meta data associate with the GameWrapper instance to an ini file.
		void writeGameMeta(std::string);

	public:

		// Returns raw meta data associated with a dumped game
		static std::unordered_map<std::string, std::string> getDumpedGameMeta(
			const std::string &metaPath
		);

		// Constructor
		GameWrapper() = delete;
		GameWrapper(const GameWrapper &) = delete;

		// Constructor takes as input a definition, name, and any optional
		// meta data that the game should be initialized with and creates an
		// internal instance of trogdor::Game.
		GameWrapper(
			size_t gid,
			std::string &definitionPath,
			std::string &name,
			std::unordered_map<std::string, std::string> meta = {}
		);

		// Deserialization constructor, which takes as input the path to data
		// previously dumped by GameWrapper::dump() and an optional save slot.
		// If a save slot isn't specified, we'll restore the most recent
		// dump. Throws an instance of UnsupportedOperation if the state
		// feature is disabled, GameNotFound if the dumped game id isn't found,
		// GameSlotNotFound if a slot was specified but doesn't exist,
		// trogdor::UndefinedException if the data to deserialize is invalid,
		// and ServerException for any other error.
		GameWrapper(const STD_FILESYSTEM::path &p, std::optional<size_t> slot = std::nullopt);

		// Serializes and saves the state of the game, along with any
		// associated meta data and returns the dump slot where the game was
		// saved (if state is disabled, this always returns 0 with no effect.)
		size_t dump();

		// Deletes all dumped data for the given game.
		void destroyDump();

		// Returns a reference to the underlying game
		inline std::unique_ptr<trogdor::Game> &get() {return gamePtr;}

		// Returns the game's id
		inline const size_t getId() const {return id;}

		// Returns the game's name
		inline const std::string getName() const {return name;}

		// Return the game definition path that was used to instantiate the game
		inline const std::string getDefinition() const {return definition;}

		// Returns the time the game was created
		inline const std::time_t &getCreated() const {return created;}

		// Returns the current number of players in the game
		inline const size_t getNumPlayers() const {return gamePtr->getPlayers().size();}

		// Returns the dump slot that was restored for this game (if any)
		std::optional<size_t> getRestoredSlot() const {return restoredSlot;}
};

#endif
