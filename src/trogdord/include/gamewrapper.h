#ifndef GAME_WRAPPER_H
#define GAME_WRAPPER_H

#include <memory>
#include <trogdor/game.h>

class GameWrapper {

	private:

		// The game's name
		std::string name;

		// The actual underlying game object
		std::unique_ptr<trogdor::Game> gamePtr;

	public:

		GameWrapper() = delete;
		GameWrapper(const GameWrapper &) = delete;

		// Constructor takes as input a definition, name, and any optional
		// meta data that the game should be initialized with and creates an
		// internal instance of trogdor::Game.
		GameWrapper(
			std::string definitionPath,
			std::string name,
			std::unordered_map<std::string, std::string> meta = {}
		);

		// Returns a reference to the underlying game
		inline std::unique_ptr<trogdor::Game> &get() {return gamePtr;}

		// Returns the game's name
		inline const std::string getName() const {return name;}
};

#endif
