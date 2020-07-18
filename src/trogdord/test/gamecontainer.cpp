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

	TEST_CASE("GameContainer (gamecontainer.cpp): createGame(), getGame(), destroyGame(), getMetaAll(), and size()") {

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

			// Remove the remaining game and verify that the size is now 0 again
			container->destroyGame(gameId2);

			CHECK(0 == container->size());
			CHECK(nullptr == container->getGame(gameId2));

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): startGame() and stopGame()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			GameContainer::reset();
			auto &container = GameContainer::get();

			size_t gameId;

			try {
				gameId = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				FAIL(std::string("Failed to initialize game: ") + e.what());
			}

			container->startGame(gameId);
			CHECK(container->getGame(gameId)->get()->inProgress());

			container->stopGame(gameId);
			CHECK(!container->getGame(gameId)->get()->inProgress());

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getMeta() and setMeta()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			GameContainer::reset();
			auto &container = GameContainer::get();

			size_t gameId;

			try {
				gameId = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				FAIL(std::string("Failed to initialize game: ") + e.what());
			}

			// getMeta() for unset value should return an empty string
			CHECK(0 == container->getMeta(gameId, "a_meta_key").compare(""));

			// Make sure values set by setMeta() can be retrieved
			container->setMeta(gameId, "a_meta_key", "a value");
			CHECK(0 == container->getMeta(gameId, "a_meta_key").compare("a value"));

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getNumPlayers()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;


			GameContainer::reset();
			auto &container = GameContainer::get();

			size_t gameId1;
			size_t gameId2;

			try {
				gameId1 = container->createGame(definition, gameName);
				gameId2 = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				FAIL(std::string("Failed to initialize game: ") + e.what());
			}

			try {

				// Make sure we start off with 0 players
				CHECK(0 == container->getNumPlayers());

				// Verify that getNumPlayers always returns the combined total of
				// players in all games
				container->createPlayer(gameId1, "player1");
				CHECK(1 == container->getNumPlayers());

				container->createPlayer(gameId2, "player2");
				CHECK(2 == container->getNumPlayers());

				container->removePlayer(gameId1, "player1");
				CHECK(1 == container->getNumPlayers());

				container->removePlayer(gameId2, "player2");
				CHECK(0 == container->getNumPlayers());
			}

			catch (const std::exception &e) {
				FAIL(e.what());
			}

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): createPlayer() and removePlayer()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			SUBCASE("Inserting into and removing from a stopped game") {

				GameContainer::reset();
				auto &container = GameContainer::get();

				// Step 1: create a game
				size_t gameId;

				try {
					gameId = container->createGame(definition, gameName);
				}

				catch (const ServerException &e) {
					FAIL(std::string("Failed to initialize game: ") + e.what());
				}

				// Make sure game is stopped (that should be the default,
				// but let's explicitly stop it anyway.)
				container->stopGame(gameId);

				// Step 2: Create a player in the game an verify it exists
				container->createPlayer(gameId, "player");
				CHECK(container->getGame(gameId)->get()->getPlayer("player"));

				// Step 3: remove the player and verify it was removed
				container->removePlayer(gameId, "player");
				CHECK(!container->getGame(gameId)->get()->getPlayer("player"));

			}

			SUBCASE("Inserting into and removing from a started game") {

				GameContainer::reset();
				auto &container = GameContainer::get();

				// Step 1: create a game an start it
				size_t gameId;

				try {
					gameId = container->createGame(definition, gameName);
				}

				catch (const ServerException &e) {
					FAIL(std::string("Failed to initialize game: ") + e.what());
				}

				container->startGame(gameId);

				// Step 2: Create a player in the game an verify it exists
				container->createPlayer(gameId, "player");
				CHECK(container->getGame(gameId)->get()->getPlayer("player"));

				// Step 3: remove the player and verify it was removed
				container->removePlayer(gameId, "player");
				CHECK(!container->getGame(gameId)->get()->getPlayer("player"));
			}

			SUBCASE("Inserting into a stopped game, then removing after the game is started") {

				GameContainer::reset();
				auto &container = GameContainer::get();

				// Step 1: create a game
				size_t gameId;

				try {
					gameId = container->createGame(definition, gameName);
				}

				catch (const ServerException &e) {
					FAIL(std::string("Failed to initialize game: ") + e.what());
				}

				// Make sure game is stopped (that should be the default,
				// but let's explicitly stop it anyway.)
				container->stopGame(gameId);

				// Step 2: Create a player in the game an verify it exists
				container->createPlayer(gameId, "player");
				CHECK(container->getGame(gameId)->get()->getPlayer("player"));

				// Step 3: start the game after the player was created
				container->startGame(gameId);

				// Step 4: remove the player and verify it was removed
				container->removePlayer(gameId, "player");
				CHECK(!container->getGame(gameId)->get()->getPlayer("player"));
			}

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getGames()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			SUBCASE("No filters") {

				GameContainer::reset();
				auto &container = GameContainer::get();

				size_t startedId = container->createGame(definition, "started");
				container->createGame(definition, "stopped");

				container->startGame(startedId);
				CHECK(2 == container->getGames().size());
			}

			SUBCASE("Single filter") {

				GameContainer::reset();
				auto &container = GameContainer::get();

				// Should only return games that are running
				Filter::Union filter = {{
					{"is_running", true}
				}};

				size_t startedId = container->createGame(definition, "started");
				container->createGame(definition, "stopped");

				container->startGame(startedId);

				auto games = container->getGames(filter);

				CHECK(1 == games.size());
				CHECK(startedId == *games.begin());
			}

			SUBCASE("Filter group") {

				GameContainer::reset();
				auto &container = GameContainer::get();

				// Should only return games that are started AND whose names
				// start with the substring "a"
				Filter::Union group = {{
					{"is_running", true},
					{"name_starts", std::string("a")}
				}};

				size_t matchingId1 = container->createGame(definition, "a cool game");
				size_t nonmatchingId1 = container->createGame(definition, "not a cool game");
				container->createGame(definition, "not a cool game");
				container->createGame(definition, "a cool game");

				container->startGame(matchingId1);
				container->startGame(nonmatchingId1);

				auto games = container->getGames(group);

				CHECK(1 == games.size());
				CHECK(games.end() != games.find(matchingId1));
			}

			SUBCASE("Filter union") {

				GameContainer::reset();
				auto &container = GameContainer::get();

				// Should only return games that are started OR games whose
				// name starts with the substring "a"
				Filter::Union funion = {
					{{"is_running", true}},
					{{"name_starts", std::string("a")}}
				};

				size_t matchingId1 = container->createGame(definition, "a cool game");
				size_t matchingId2 = container->createGame(definition, "not a cool game");
				container->createGame(definition, "not a cool game");
				size_t matchingId3 = container->createGame(definition, "a cool game");

				container->startGame(matchingId1);
				container->startGame(matchingId2);

				auto games = container->getGames(funion);

				CHECK(3 == games.size());
				CHECK(games.end() != games.find(matchingId1));
				CHECK(games.end() != games.find(matchingId2));
				CHECK(games.end() != games.find(matchingId3));
			}

		#endif
	}

	// I have issues with this scenario causing segmentation faults, so this is
	// my attempt to reliably reproduce and automate this issue so I can debug
	// it and figure out what's wrong.
	// WARNING: if running these tests as part of an automation process, invoke
	// the test_trogdord executable indirectly with timeout (e.g.
	// timeout 1 ./test_trogdord). This is because the issue I'm running into
	// is a deadlock in a thread, causing a segfault when I do CTRL-C.
	TEST_CASE("GameContainer (gamecontainer.cpp): Quitting application while games with players are running") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;


			GameContainer::reset();
			auto &container = GameContainer::get();

			// Step 1: create and start a game
			size_t gameId;

			try {
				gameId = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				FAIL(std::string("Failed to initialize game: ") + e.what());
			}

			container->startGame(gameId);

			// Step 2: Create a player in the game
			container->createPlayer(gameId, "player");

			// Step 3: Simulate the closing of the application by resetting
			// the singleton instance. Because the issue I'm reproducing is
			// a deadlock in a thread that causes a crash on CTRL-C, if
			// automating these tests, be sure to invoke the test_trogdord
			// target indirectly using timeout (e.g. timeout 1 ./test_trogdord)
			GameContainer::reset();

		#endif
	}
}
