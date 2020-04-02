#ifndef GAME_WRAPPER_H
#define GAME_WRAPPER_H

#include <memory>
#include <trogdor/game.h>

class GameWrapper {

	private:

		std::unique_ptr<trogdor::Game> gamePtr;

	public:

		// Constructor
		GameWrapper() = delete;
		GameWrapper(const GameWrapper &) = delete;

		inline GameWrapper(std::unique_ptr<trogdor::Game> &g): gamePtr(std::move(g)) {}

		// Returns a reference to the underlying game
		inline std::unique_ptr<trogdor::Game> &get() {return gamePtr;}
};

#endif
