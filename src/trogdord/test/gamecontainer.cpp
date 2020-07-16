#include <doctest.h>
#include "../include/gamecontainer.h"


TEST_SUITE("GameContainer (gamecontainer.cpp)") {

	TEST_CASE("GameContainer (gamecontainer.cpp): Constructor (indirect test using get() after call to reset())") {

		GameContainer::reset();
		auto &container = GameContainer::get();

		// Check for sane initialization
		CHECK(0 == container->size());
		CHECK(0 == container->getGames().size());
		CHECK(0 == container->getNumPlayers());
	}
}
