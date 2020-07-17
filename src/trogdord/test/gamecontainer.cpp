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

	TEST_CASE("GameContainer (gamecontainer.cpp): createGame(), getGame(), destroyGame(), and size()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			// This is the number of meta values created by the game
			// definition file we're using to test. If any of the unit tests
			// related to meta fail, check this and make sure it's correct.
			// Not ideal, but I'll think of something later.
			const int defaultMetaSize = 2;

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			GameContainer::reset();
			auto &container = GameContainer::get();

			// Number of games should be 0 until one or more are created
			CHECK(0 == container->size());

			// Test createGame() without metadata
			size_t gameId1;

			try {
				gameId1 = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				FAIL(std::string("Failed to initialize game without metadata: ") + e.what());
			}

			CHECK(container->getGame(gameId1) != nullptr);
			CHECK(0 == container->getGame(gameId1)->getName().compare(gameName));
			CHECK(defaultMetaSize == container->getGame(gameId1)->get()->getMetaAll().size());
			CHECK(1 == container->size());

			// Test createGame() with metadata
			size_t gameId2;

			try {
				gameId2 = container->createGame(definition, gameName, {{"key", "value"}});
			}

			catch (const ServerException &e) {
				FAIL(std::string("Failed to initialize game with metadata: ") + e.what());
			}

			CHECK(container->getGame(gameId2) != nullptr);
			CHECK(0 == container->getGame(gameId2)->getName().compare(gameName));
			CHECK(defaultMetaSize + 1 == container->getGame(gameId2)->get()->getMetaAll().size());
			CHECK(0 == container->getGame(gameId2)->get()->getMeta("key").compare("value"));
			CHECK(2 == container->size());

			// Verify that getting a non-existent game returns nullptr
			CHECK(nullptr == container->getGame(gameId2 + 1));

			container->destroyGame(gameId1);

			// Make sure only the correct game was destroyed
			CHECK(1 == container->size());
			CHECK(nullptr == container->getGame(gameId1));
			CHECK(nullptr != container->getGame(gameId2));

			container->destroyGame(gameId1);

			// Verify that calling destroy again on the same id does nothing
			CHECK(1 == container->size());
			CHECK(nullptr == container->getGame(gameId1));
			CHECK(nullptr != container->getGame(gameId2));

			// Calling destroyGame() on a non-existent id should do nothing
			// and not throw an error
			container->destroyGame(gameId2 + 1);

			container->destroyGame(gameId2);

			// Remove the remaining game and verify that the size is now 0 again
			CHECK(0 == container->size());
			CHECK(nullptr == container->getGame(gameId2));

		#endif
	}
}
