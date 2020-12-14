#ifndef GAME_WRAPPER_H
#define GAME_WRAPPER_H

#include <memory>
#include <ctime>
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

		// Returns a serialized version of all meta data associate with the
		// GameWrapper instance.
		std::shared_ptr<trogdor::serial::Serializable> serializeMeta();

	public:

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
		// previously dumped by GameWrapper::dump().
		GameWrapper(const STD_FILESYSTEM::path &p);

		// Serializes and saves the state of the game, along with any
		// associated meta data.
		void dump();

		// Deletes all dumped data for the given game.
		void destroyDump();

		// Returns a reference to the underlying game
		inline std::unique_ptr<trogdor::Game> &get() {return gamePtr;}

		// Returns the game's name
		inline const std::string getName() const {return name;}

		// Return the game definition path that was used to instantiate the game
		inline const std::string getDefinition() const {return definition;}

		// Returns the time the game was created
		inline const std::time_t &getCreated() const {return created;}

		// Returns the current number of players in the game
		inline const size_t getNumPlayers() const {return gamePtr->getPlayers().size();}
};

#endif
