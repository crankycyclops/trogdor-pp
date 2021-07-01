#include <doctest.h>

#include "config.h"
#include "scopes/helper.h"

#include <trogdord/gamecontainer.h>
#include <trogdord/exception/unsupportedoperation.h>
#include <trogdord/exception/gameslotnotfound.h>


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
				std::string message = std::string("Failed to initialize game without metadata: ") + e.what();
				FAIL(message);
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
				std::string message = std::string("Failed to initialize game with metadata: ") + e.what();
				FAIL(message);
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
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
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
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
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
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
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
					std::string message = std::string("Failed to initialize game: ") + e.what();
					FAIL(message);
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
					std::string message = std::string("Failed to initialize game: ") + e.what();
					FAIL(message);
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
					std::string message = std::string("Failed to initialize game: ") + e.what();
					FAIL(message);
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
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
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

	TEST_CASE("GameContainer (gamecontainer.cpp): Destroying running game with players") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::chrono::milliseconds threadSleepTime(tickInterval * 3);

			GameContainer::reset();
			auto &container = GameContainer::get();

			// Step 1: create and start a game
			size_t gameId;

			try {
				gameId = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
			}

			container->startGame(gameId);

			// Step 2: Create a player in the game and wait a little while
			container->createPlayer(gameId, "player");
			std::this_thread::sleep_for(threadSleepTime);

			// Step 3: Destroy game
			container->destroyGame(gameId);

			// We're just making sure we don't deadlock or segfault
			CHECK(true);

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Destroying running game that had players but doesn't anymore") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::chrono::milliseconds threadSleepTime(tickInterval * 3);

			GameContainer::reset();
			auto &container = GameContainer::get();

			// Step 1: create and start a game
			size_t gameId;

			try {
				gameId = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
			}

			container->startGame(gameId);

			// Step 2: Create a player in the game, wait a little while, then
			// remove it
			container->createPlayer(gameId, "player");
			std::this_thread::sleep_for(threadSleepTime);
			container->removePlayer(gameId, "player");

			// Step 3: Destroy game
			container->destroyGame(gameId);

			// We're just making sure we don't deadlock or segfault
			CHECK(true);

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Spin up many games, each one with a player, then simulate closing the application by calling GameContainer::reset()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			// The number of games to spin up during this test
			const int numGames = 10;

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::chrono::milliseconds threadSleepTime(tickInterval * 4);

			GameContainer::reset();
			auto &container = GameContainer::get();

			// Step 1: spin up a crap ton of games, start them, and insert a
			// player into each
			std::vector<size_t> gameIds;

			try {

				for (int i = 0; i < numGames; i++) {

					size_t gameId = container->createGame(definition, gameName);

					container->startGame(gameId);
					gameIds.push_back(gameId);

					container->createPlayer(gameId, "player");
				}
			}

			catch (const ServerException &e) {
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
			}

			// Wait for a little while to give threads time to do their thing
			std::this_thread::sleep_for(threadSleepTime);

			// Simulate closing the application
			GameContainer::reset();

			// We're just making sure we don't deadlock or segfault
			CHECK(true);

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Add many players to a running game, then simulate closing the application by calling GameContainer::reset()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			// The number of players to add during this test
			const int numPlayers = 25;

			std::string gameName = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::chrono::milliseconds threadSleepTime(tickInterval * 4);

			GameContainer::reset();
			auto &container = GameContainer::get();

			// Step 1: create and start a game
			size_t gameId;

			try {
				gameId = container->createGame(definition, gameName);
			}

			catch (const ServerException &e) {
				std::string message = std::string("Failed to initialize game: ") + e.what();
				FAIL(message);
			}

			// Step 2: start the game
			container->startGame(gameId);

			// Step 3: create the players
			for (int i = 0; i < numPlayers; i++) {
				container->createPlayer(gameId, std::string("player") + std::to_string(i));
			}

			// Wait for a little while to give threads time to do their thing
			std::this_thread::sleep_for(threadSleepTime);

			// Simulate closing the application
			GameContainer::reset();

			// We're just making sure we don't deadlock or segfault
			CHECK(true);

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Constructor: state enabled, but configured state path is blank") {

		initGameXML();
		initConfig(false, true, "");
		GameContainer::reset();

		try {
			GameContainer::get();
			FAIL("GameContainer construction must fail when state is enabled and the configured state path is blank.");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}

		// Restore the default configuration
		destroyGameXML();
		destroyConfig();
		initIniFile(iniFilename, {{}}); // iniFilename is defined in helper.h
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Constructor: state enabled, but configured state path doesn't exist") {

		// For this particular test, this path won't actually exist
		std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
			STD_FILESYSTEM::path::preferred_separator + "/trogstate";

		initGameXML();
		initConfig(false, true, statePath);
		GameContainer::reset();

		try {
			GameContainer::get();
			FAIL("GameContainer construction must fail when state is enabled and the configured state path doesn't exist.");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}

		// Restore the default configuration
		destroyGameXML();
		destroyConfig();
		initIniFile(iniFilename, {{}}); // iniFilename is defined in helper.h
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Constructor: state enabled, but configured state path is read-only") {

		std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
			STD_FILESYSTEM::path::preferred_separator + "/trogstate";

		// Make a read-only state directory
		STD_FILESYSTEM::create_directory(statePath);
		STD_FILESYSTEM::permissions(
			statePath,
			STD_FILESYSTEM::perms::group_read  | STD_FILESYSTEM::perms::group_exec  |
			STD_FILESYSTEM::perms::owner_read  | STD_FILESYSTEM::perms::owner_exec  |
			STD_FILESYSTEM::perms::others_read | STD_FILESYSTEM::perms::others_exec
		);

		initGameXML();
		initConfig(false, true, statePath);
		GameContainer::reset();

		try {
			GameContainer::get();
			FAIL("GameContainer construction must fail when state is enabled and the configured state path is read-only.");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}

		// Restore the default configuration
		destroyGameXML();
		destroyConfig();
		initIniFile(iniFilename, {{}}); // iniFilename is defined in helper.h
		STD_FILESYSTEM::permissions(statePath, STD_FILESYSTEM::perms::owner_write);
		STD_FILESYSTEM::remove_all(statePath);
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Constructor: state enabled, no previously dumped games") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Create valid writable state path
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Since there are no dumped game ids to reserve, we should be able
			// to get an id of 0 the first time we create a game.
			try {
				auto &container = GameContainer::get();
				size_t id = container->createGame(definition, gameName);
				CHECK(0 == id);
			}

			catch (const ServerException &e) {
				FAIL(e.what());
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}}); // iniFilename is defined in helper.h

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Constructor: state enabled, one previously dumped game") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Create valid writable state path
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);

			// Reset GameContainer to simulate trogdord startup
			GameContainer::reset();

			// Since there are no dumped game ids to reserve, we should be able
			// to get an id of 0 the first time we create a game.
			try {

				// Step 1: create and dump a game
				auto &container = GameContainer::get();
				size_t id = container->createGame(definition, gameName);
				CHECK(0 == id);

				// Dump all existing games to disk (will just be the one)
				container->dump();

				// Pretend the application was shutdown and restarted
				GameContainer::reset();
				id = GameContainer::get()->createGame(definition, gameName);

				// Since id 0 should have been reserved due to it having
				// been dumped, the next id generated should be 1.
				CHECK(1 == id);
			}

			catch (const ServerException &e) {
				FAIL(e.what());
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): dump() and restore()") {

		SUBCASE("State disabled, zero games") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Methods should just return without actually doing anything
			GameContainer::get()->dump();
			GameContainer::reset();
			GameContainer::get()->restore();

			CHECK(0 == GameContainer::get()->getGames().size());

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State disabled, one game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, false, statePath);
				GameContainer::reset();

				// Create a game and demonstrate that it doesn't get dumped when
				// state is disabled.
				GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				// If we encounter any subdirectories, we know we dun screwed up!
				for (const auto &subdir: STD_FILESYSTEM::directory_iterator(statePath)) {
					std::string message = subdir.path().filename().string() + ": no games should be dumped when state is disabled.";
					FAIL(message);
				}

				// Verify that after restoring, we have zero games, since nothing
				// should be dumped or restored when state is disabled.
				GameContainer::reset();
				GameContainer::get()->restore();

				CHECK(0 == GameContainer::get()->getGames().size());

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled, zero games") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			GameContainer::get()->dump();

			// We didn't create a game, so this state directory should
			// remain empty
			for (const auto &subdir: STD_FILESYSTEM::directory_iterator(statePath)) {
				std::string message = subdir.path().filename().string() + ": no games should be dumped when state is disabled.";
				FAIL(message);
			}

			// Verify that after restoring, we have zero games, since nothing
			// exited before that could be dumped.
			GameContainer::reset();
			GameContainer::get()->restore();

			CHECK(0 == GameContainer::get()->getGames().size());

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State enabled, one game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game with a player and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);

				GameContainer::get()->createPlayer(id, "test");
				GameContainer::get()->dump();

				// Make sure the dumped id matches the game id and that
				// there's only one subdirectory to represent the dumped game.
				size_t count = 0;

				for (const auto &subdir: STD_FILESYSTEM::directory_iterator(statePath)) {

					count++;

					if (0 != std::to_string(id).compare(subdir.path().filename().string())) {
						std::string message = "Game id " + std::to_string(id) + " doesn't match dumped id " + subdir.path().filename().string();
						FAIL(message);
					}
				}

				CHECK(1 == count);

				// Verify that after restoring, we get the same game back
				// with the player we created.
				GameContainer::reset();
				GameContainer::get()->restore();

				CHECK(1 == GameContainer::get()->getGames().size());
				CHECK(GameContainer::get()->getGame(id));
				CHECK(1 == GameContainer::get()->getGame(id)->getNumPlayers());
				CHECK(GameContainer::get()->getGame(id)->get()->getPlayer("test"));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): destroyGame()") {

		SUBCASE("State disabled (shouldn't touch previously dumped games)") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				/////////////////////////////////////////////////////
				// Part 1: Create a dumped game with state enabled //
				/////////////////////////////////////////////////////

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				// Verify that the game was dumped
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				////////////////////////////////////////////////////
				// Part 2: Restart the server with state disabled //
				////////////////////////////////////////////////////

				destroyConfig();

				initGameXML();
				initConfig(false, false, statePath);
				GameContainer::reset();

				size_t id2 = GameContainer::get()->createGame(definition, gameName);

				// First, verify that, with state disabled, no ids are reserved and
				// that the two ids therefore match.
				CHECK(id == id2);

				// Next, call GameContainer::destroyGame() with the destroyDump
				// argument explicitly set to true and observe that, with state
				// disabled, the previously game remains untouched.
				GameContainer::get()->destroyGame(id2);
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled: destroyDump argument should be true by default") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				size_t id = GameContainer::get()->createGame(definition, gameName);
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				GameContainer::get()->dump();
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// By default, the dump should be destroyed along with the game
				GameContainer::get()->destroyGame(id);
				CHECK(!STD_FILESYSTEM::exists(gameStatePath));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("state enabled: destroy dumped games with destroyDump set explicitly to true"){

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				size_t id = GameContainer::get()->createGame(definition, gameName);
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				GameContainer::get()->dump();
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// The dump should be destroyed along with the game when we
				// set destroyDump argument to true.
				GameContainer::get()->destroyGame(id, true);
				CHECK(!STD_FILESYSTEM::exists(gameStatePath));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled: optionally preserve dumped games") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				size_t id = GameContainer::get()->createGame(definition, gameName);
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				GameContainer::get()->dump();
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// By setting destroyDump argument to false, we should be
				// able to delete the game but keep its dump.
				GameContainer::get()->destroyGame(id, false);
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): restoreGame()") {

		SUBCASE("restoreGame(): state disabled, dumped game doesn't exist") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Without a game slot
			try {
				GameContainer::get()->restoreGame(0);
				FAIL("GameContainer::restoreGame() should fail if state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// With a game slot
			try {
				GameContainer::get()->restoreGame(0, 0);
				FAIL("GameContainer::restoreGame() should fail if state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("restoreGame(): state disabled, dumped game exists") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				/////////////////////////////////////////////////////
				// Part 1: Create a dumped game with state enabled //
				/////////////////////////////////////////////////////

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);
				size_t slot = GameContainer::get()->getGame(id)->dump();

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				// Verify that the game was dumped
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				////////////////////////////////////////////////////
				// Part 2: Restart the server with state disabled //
				////////////////////////////////////////////////////

				STD_FILESYSTEM::remove(iniFilename);

				initGameXML();
				initConfig(false, false, statePath);
				GameContainer::reset();

				// Without a game slot
				try {
					GameContainer::get()->restoreGame(id);
					FAIL("GameContainer::restoreGame() should fail if state is disabled.");
				}

				catch (const UnsupportedOperation &e) {
					CHECK(true);
				}

				// With a game slot
				try {
					GameContainer::get()->restoreGame(id, slot);
					FAIL("GameContainer::restoreGame() should fail if state is disabled.");
				}

				catch (const UnsupportedOperation &e) {
					CHECK(true);
				}

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled, game doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Without a game slot
			try {
				GameContainer::get()->restoreGame(0);
				FAIL("GameContainer::restoreGame() should fail if the game isn't found.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			// With a game slot
			try {
				GameContainer::get()->restoreGame(0, 0);
				FAIL("GameContainer::restoreGame() should fail if state is disabled.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		// TODO: test indexes via GameContainer::getGames() with appropriate
		// filter unions.
		SUBCASE("State enabled, game exists, reset server between resets") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Version 1 of the game will be stopped and have no players
				size_t id = GameContainer::get()->createGame(definition, gameName);
				size_t oldSlot = GameContainer::get()->getGame(id)->dump();

				// Version 2 of the game will be started and have one player
				GameContainer::get()->startGame(id);
				GameContainer::get()->createPlayer(id, "player");
				size_t newSlot = GameContainer::get()->getGame(id)->dump();

				GameContainer::reset();
				CHECK(0 == GameContainer::get()->getGames().size());

				// By default, restoreGame() should restore the most recent
				// dump slot, which we verify by checking to make sure it's
				// started and that a player was created.
				size_t restoredSlot = GameContainer::get()->restoreGame(id);

				CHECK(newSlot == restoredSlot);
				CHECK(true == GameContainer::get()->getGame(id)->get()->inProgress());
				CHECK(1 == GameContainer::get()->getGame(id)->getNumPlayers());

				GameContainer::reset();
				CHECK(0 == GameContainer::get()->getGames().size());

				// Next, we load the first dumped slot, which shouldn't yet
				// be started and shouldn't yet have a player.
				restoredSlot = GameContainer::get()->restoreGame(id, oldSlot);

				CHECK(oldSlot == restoredSlot);
				CHECK(false == GameContainer::get()->getGame(id)->get()->inProgress());
				CHECK(0 == GameContainer::get()->getGame(id)->getNumPlayers());

				GameContainer::reset();
				CHECK(0 == GameContainer::get()->getGames().size());

				// Finally, attempt to restore a dump slot that doesn't exist
				try {
					GameContainer::get()->restoreGame(id, 100);
					FAIL("GameContainer::restoreGame() should fail when passed a dump slot that doesn't exist.");
				}

				catch (const GameSlotNotFound &e) {
					CHECK(true);
				}

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		// TODO: test indexes via GameContainer::getGames() with appropriate
		// filter unions.
		SUBCASE("State enabled, game exists, don't reset server between resets") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Version 1 of the game will be stopped and have no players
				size_t id = GameContainer::get()->createGame(definition, gameName);
				size_t oldSlot = GameContainer::get()->getGame(id)->dump();

				// Version 2 of the game will be started and have one player
				GameContainer::get()->startGame(id);
				GameContainer::get()->createPlayer(id, "player");
				size_t newSlot = GameContainer::get()->getGame(id)->dump();

				// By default, restoreGame() should restore the most recent
				// dump slot, which we verify by checking to make sure it's
				// started and that a player was created.
				size_t restoredSlot = GameContainer::get()->restoreGame(id);

				CHECK(newSlot == restoredSlot);
				CHECK(true == GameContainer::get()->getGame(id)->get()->inProgress());
				CHECK(1 == GameContainer::get()->getGame(id)->getNumPlayers());

				// Next, we load the first dumped slot, which shouldn't yet
				// be started and shouldn't yet have a player.
				restoredSlot = GameContainer::get()->restoreGame(id, 0);

				CHECK(oldSlot == restoredSlot);
				CHECK(false == GameContainer::get()->getGame(id)->get()->inProgress());
				CHECK(0 == GameContainer::get()->getGame(id)->getNumPlayers());

				// Finally, attempt to restore a dump slot that doesn't exist
				try {
					GameContainer::get()->restoreGame(id, 100);
					FAIL("GameContainer::restoreGame() should fail when passed a dump slot that doesn't exist.");
				}

				catch (const GameSlotNotFound &e) {
					CHECK(true);
				}

				// Verify that the game wasn't destroyed as a result of the
				// failed restore.
				CHECK(1 == GameContainer::get()->getGames().size());
				CHECK(nullptr != GameContainer::get()->getGame(id));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getDumpedGameIds()") {

		SUBCASE("State feature is disabled, no dumped games") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			CHECK(0 == GameContainer::get()->getDumpedGameIds().size());

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State feature is disabled, one dumped game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				/////////////////////////////////////////////////////
				// Part 1: Create a dumped game with state enabled //
				/////////////////////////////////////////////////////

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game with a player and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				// Verify that the game was dumped
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				////////////////////////////////////////////////////
				// Part 2: Restart the server with state disabled //
				////////////////////////////////////////////////////

				STD_FILESYSTEM::remove(iniFilename);

				initGameXML();
				initConfig(false, false, statePath);
				GameContainer::reset();

				CHECK(0 == GameContainer::get()->getDumpedGameIds().size());

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State feature is enabled, no dumped games") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Since we didn't dump any games, we shouldn't get any ids back
			CHECK(0 == GameContainer::get()->getDumpedGameIds().size());

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State feature is enabled, one dumped game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game with a player and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);

				// Show that we don't return true until after it's been dumped
				CHECK(false == GameContainer::get()->isDumpedGameId(id));

				GameContainer::get()->dump();

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				// Verify that the game was dumped
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// We should now get back a list of size 1 containing the
				// game id we just dumped.
				CHECK(1 == GameContainer::get()->getDumpedGameIds().size());
				CHECK(id == *GameContainer::get()->getDumpedGameIds().begin());

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): isDumpedGameId()") {

		SUBCASE("State feature is disabled, dumped game doesn't exist") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Should always return false when state is disabled
			CHECK(false == GameContainer::get()->isDumpedGameId(0));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State feature is disabled, dumped game exists") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				/////////////////////////////////////////////////////
				// Part 1: Create a dumped game with state enabled //
				/////////////////////////////////////////////////////

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game with a player and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				// Verify that the game was dumped
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				////////////////////////////////////////////////////
				// Part 2: Restart the server with state disabled //
				////////////////////////////////////////////////////

				STD_FILESYSTEM::remove(iniFilename);

				initGameXML();
				initConfig(false, false, statePath);
				GameContainer::reset();

				// Method always returns false when state feature is disabled
				CHECK(false == GameContainer::get()->isDumpedGameId(id));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State feature is enabled, dumped game doesn't exist") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game with a player and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				// Verify that the game was dumped
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// Now, we're going to check an id that we know for a fact
				// doesn't exist (id + 1).
				CHECK(false == GameContainer::get()->isDumpedGameId(id + 1));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State feature is enabled, dumped game exists") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create a game and demonstrate that it gets dumped.
				size_t id = GameContainer::get()->createGame(definition, gameName);

				// Show that we don't return true until after it's been dumped
				CHECK(false == GameContainer::get()->isDumpedGameId(id));

				GameContainer::get()->dump();

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

				// Verify that the game was dumped
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// This should now return true
				CHECK(true == GameContainer::get()->isDumpedGameId(id));

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getDumpedGame()") {

		SUBCASE("getDumpedGame(): state disabled, dumped game id doesn't exist") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGame(0);
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("getDumpedGame(): state disabled, dumped game exists but is invalid") {

			size_t gameId = 0;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Create state directory
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			std::string gameStatePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

			// Make an empty and invalid "dumped game"
			STD_FILESYSTEM::create_directory(gameStatePath);

			try {
				GameContainer::get()->getDumpedGame(gameId);
				FAIL("State is disabled, so call to GameContainer::dumpedGame() should not succeed.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("getDumpedGame(): state disabled, dumped game exists and is valid") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
						STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Create state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, false);
				GameContainer::reset();

				// Create and dump a game
				size_t id = GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				try {
					GameContainer::get()->getDumpedGame(id);
					FAIL("State is disabled, so call to GameContainer::dumpedGame() should not succeed.");
				}

				catch (const UnsupportedOperation &e) {
					CHECK(true);
				}

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				initIniFile(iniFilename, {{}});
				STD_FILESYSTEM::remove_all(statePath);

			#endif
		}

		SUBCASE("getDumpedGame(): state enabled, dumped game id doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Create state directory
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGame(0);
				FAIL("Call to GameContainer::getDumpedGame() should fail when the dumped game doesn't exist.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("getDumpedGame(): state enabled, dumped game exists but is invalid") {

			size_t gameId = 0;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Create state path
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			std::string gameStatePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

			// Make an empty and invalid "dumped game"
			STD_FILESYSTEM::create_directory(gameStatePath);

			try {
				GameContainer::get()->getDumpedGame(gameId);
				FAIL("Call to GameContainer::getDumpedGame() should fail when the game doesn't exist or is invalid.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("getDumpedGame(): state enabled, dumped game exists and is valid") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
						STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				// Create state directory
				STD_FILESYSTEM::create_directory(statePath);

				initGameXML();
				initConfig(false, true, statePath);
				GameContainer::reset();

				// Create and dump a game
				size_t id = GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				auto data = GameContainer::get()->getDumpedGame(id);

				CHECK(0 == std::get<0>(data).compare(gameName));
				CHECK(0 == std::get<1>(data).compare("game.xml")); // TODO: derive from CORE_UNIT_TEST_DEFINITION_FILE
				CHECK(std::get<2>(data) > 0);

				// Restore the default configuration
				destroyGameXML();
				destroyConfig();
				initIniFile(iniFilename, {{}});
				STD_FILESYSTEM::remove_all(statePath);

			#endif
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getDumpedGameSlot()") {

		SUBCASE("State disabled, dumped game doesn't exist") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGameSlot(0, 0);
				FAIL("GameContainer::getDumpedGameSlot() should always throw UnsupportedOperation when state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State disabled, dumped game id is an empty directory") {

			size_t id = 0;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Create a fake (empty) game dump directory and attempt to get its details
			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			STD_FILESYSTEM::create_directory(gamePath);

			try {
				GameContainer::get()->getDumpedGameSlot(id, 0);
				FAIL("GameContainer::getDumpedGameSlot() should throw UnsupportedOperation.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game id exists but slot does not") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			// Now, disable state and attempt to get a non-existing dump slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot + 1);
				FAIL("GameContainer::getDumpedGameSlot() should always throw UnsupportedOperation when state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game slot is an empty directory") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game so that I have a valid dump directory
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			// Create a second empty (invalid) dump slot
			std::string slotPath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id) +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot + 1);

			STD_FILESYSTEM::create_directory(slotPath);

			// Now, disable state and attempt to get an invalid (empty) dump slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot + 1);
				FAIL("GameContainer::getDumpedGameSlot() should always throw UnsupportedOperation when state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game slot timestamp file is a directory") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string timestampPath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id) +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot) +
				STD_FILESYSTEM::path::preferred_separator + "timestamp";

			// Replace slot timestamp with a directory, so we can demonstrate
			// that it's caught as an invalid slot
			STD_FILESYSTEM::remove(timestampPath);
			STD_FILESYSTEM::create_directory(timestampPath);

			// Now, disable state and attempt to get a non-existing dump slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot);
				FAIL("GameContainer::getDumpedGameSlot() should always throw UnsupportedOperation when state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game slot timestamp contains non-timestamp data") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string timestampPath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id) +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot) +
				STD_FILESYSTEM::path::preferred_separator + "timestamp";

			// Replace slot timestamp with invalid data so we can demonstrate
			// that it's caught as an invalid slot
			STD_FILESYSTEM::remove(timestampPath);

			std::ofstream timestampFile(timestampPath);
			timestampFile << "not a timestamp" << std::endl;
			timestampFile.close();

			// Now, disable state and attempt to get a non-existing dump slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot);
				FAIL("GameContainer::getDumpedGameSlot() should always throw UnsupportedOperation when state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game slot is valid") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			// Now, disable state and attempt to get the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot);
				FAIL("GameContainer::getDumpedGameSlot() should always throw UnsupportedOperation when state is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Create state directory
			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			try {
				GameContainer::get()->getDumpedGameSlot(0, 0);
				FAIL("Call to GameContainer::getDumpedGame() should fail when the dumped game doesn't exist.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game id is an empty directory") {

			size_t id = 0;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create a fake (empty) game dump directory and attempt to get its details
			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			STD_FILESYSTEM::create_directory(gamePath);

			try {
				GameContainer::get()->getDumpedGameSlot(id, 0);
				FAIL("GameContainer::getDumpedGameSlot() should throw GameNotFound.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game id exists but slot does not") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game so that I have a valid dump directory
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot + 1);
				FAIL("GameContainer::getDumpedGameSlot() should throw GameSlotNotFound.");
			}

			catch (const GameSlotNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game slot is an empty directory") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game so that I have a valid dump directory
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			// Create a second empty (invalid) dump slot
			std::string slotPath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id) +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot + 1);

			STD_FILESYSTEM::create_directory(slotPath);

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot + 1);
				FAIL("GameContainer::getDumpedGameSlot() should throw GameSlotNotFound.");
			}

			catch (const GameSlotNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game slot timestamp file is a directory") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string timestampPath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id) +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot) +
				STD_FILESYSTEM::path::preferred_separator + "timestamp";

			// Replace slot timestamp with a directory, so we can demonstrate
			// that it's caught as an invalid slot
			STD_FILESYSTEM::remove(timestampPath);
			STD_FILESYSTEM::create_directory(timestampPath);

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot);
				FAIL("GameContainer::getDumpedGameSlot() should throw GameSlotNotFound.");
			}

			catch (const GameSlotNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game slot timestamp contains non-timestamp data") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string timestampPath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id) +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot) +
				STD_FILESYSTEM::path::preferred_separator + "timestamp";

			// Replace slot timestamp with invalid data so we can demonstrate
			// that it's caught as an invalid slot
			STD_FILESYSTEM::remove(timestampPath);

			std::ofstream timestampFile(timestampPath);
			timestampFile << "not a timestamp" << std::endl;
			timestampFile.close();

			try {
				GameContainer::get()->getDumpedGameSlot(id, slot);
				FAIL("GameContainer::getDumpedGameSlot() should throw GameSlotNotFound.");
			}

			catch (const GameSlotNotFound &e) {
				CHECK(true);
			}

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game slot is valid") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			// Verify that we can retrieve the slot's details
			auto data = GameContainer::get()->getDumpedGameSlot(id, slot);

			CHECK(std::get<0>(data) > 0);

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): destroyDump()") {

		SUBCASE("State disabled, dumped game doesn't exist") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Attempt to destroy dump that doesn't exist. There is no return
			// value. Just need to verify that no exceptions are found.
			GameContainer::get()->destroyDump(0);
			CHECK(true);

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State disabled, dumped game exists but is invalid") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, false, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + '0';

			// Create an invalid dumped game
			STD_FILESYSTEM::create_directory(gamePath);

			// destroyDump() shouldn't do anything since state is disabled
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDump(0);
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists and is valid") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			GameContainer::get()->getGame(id)->dump();

			// Now, disable state and attempt to get the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator +
				std::to_string(id);

			// Verify that destroyDump() doesn't do anything when state is disabled
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDump(id);
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Attempt to destroy dump that doesn't exist. There is no return
			// value. Just need to verify that no exceptions are found.
			GameContainer::get()->destroyDump(0);
			CHECK(true);

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists but is invalid") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + '0';

			// Create an invalid dumped game
			STD_FILESYSTEM::create_directory(gamePath);

			// destroyDump() should remove the invalid but existing dumped game directory
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDump(0);
			CHECK(!STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists and is valid") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator +
				std::to_string(id);

			// Verify that destroyDump() removes the game's dump directory
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDump(id);
			CHECK(!STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): destroyDumpSlot()") {

		SUBCASE("State disabled, dumped game doesn't exist, slot doesn't exist") {

			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Attempt to destroy dump slot that doesn't exist. There is no return
			// value. Just need to verify that no exceptions are found.
			GameContainer::get()->destroyDumpSlot(0, 0);
			CHECK(true);

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State disabled, dumped game exists but is invalid, slot doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, false, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + '0';

			// Create an invalid dumped game
			STD_FILESYSTEM::create_directory(gamePath);

			GameContainer::get()->destroyDumpSlot(0, 0);

			// Verify that the invalid game wasn't removed.
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists and is valid, slot doesn't exist") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			// Now, disable state and attempt to destroy the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator +
				std::to_string(id);

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDumpSlot(id, slot + 1);
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists but is invalid, slot exists but is invalid, no slot remains") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			initGameXML();
			initConfig(false, false, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + "0";
			std::string slotPath = gamePath + STD_FILESYSTEM::path::preferred_separator + "0";

			// Both the game and slot directories are empty, making them existing but invalid
			STD_FILESYSTEM::create_directory(statePath);
			STD_FILESYSTEM::create_directory(gamePath);
			STD_FILESYSTEM::create_directory(slotPath);

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(slotPath));
			GameContainer::get()->destroyDumpSlot(0, 0);
			CHECK(STD_FILESYSTEM::exists(slotPath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists and is valid, slot exists but is invalid, no slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			std::string newSlotPath = gamePath +
				STD_FILESYSTEM::path::preferred_separator + "1";

			// Remove the valid dump slot and replace it with one that's invalid
			STD_FILESYSTEM::remove_all(gamePath + STD_FILESYSTEM::path::preferred_separator + std::to_string(slot));
			STD_FILESYSTEM::create_directory(newSlotPath);

			// Now, disable state and attempt to destroy the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(newSlotPath));
			GameContainer::get()->destroyDumpSlot(id, 1);
			CHECK(STD_FILESYSTEM::exists(newSlotPath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists but is invalid, slot exists but is invalid, second slot remains") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			initGameXML();
			initConfig(false, false, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + "0";
			std::string slot1Path = gamePath + STD_FILESYSTEM::path::preferred_separator + "0";
			std::string slot2Path = gamePath + STD_FILESYSTEM::path::preferred_separator + "1";

			// Both the game and slot directories are empty, making them existing but invalid
			STD_FILESYSTEM::create_directory(statePath);
			STD_FILESYSTEM::create_directory(gamePath);
			STD_FILESYSTEM::create_directory(slot1Path);
			STD_FILESYSTEM::create_directory(slot2Path);

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(0, 0);
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists and is valid, slot exists but is invalid, second slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot1 = GameContainer::get()->getGame(id)->dump();
			size_t slot2 = slot1 + 1;

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);
			std::string slot1Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot1);
			std::string slot2Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot2);

			// Create a second invalid dump slot
			STD_FILESYSTEM::create_directory(slot2Path);

			// Now, disable state and attempt to destroy the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(id, 1);
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists but is invalid, slot exists and is valid, no slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			// Removing the dumped game's meta file makes the game invalid
			STD_FILESYSTEM::remove(gamePath + STD_FILESYSTEM::path::preferred_separator + "meta");

			// Now, disable state and attempt to destroy the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDumpSlot(id, slot);
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists and is valid, slot exists and is valid, no slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			// Now, disable state and attempt to destroy the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDumpSlot(id, slot);
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists but is invalid, slot exists and is valid, second slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot1 = GameContainer::get()->getGame(id)->dump();
			size_t slot2 = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);
			std::string slot1Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot1);
			std::string slot2Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot2);

			// Removing the dumped game's meta file makes the game invalid
			STD_FILESYSTEM::remove(gamePath + STD_FILESYSTEM::path::preferred_separator + "meta");

			// Now, disable state and attempt to destroy the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(id, slot1);
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State disabled, dumped game exists and is valid, slot exists and is valid, second slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot1 = GameContainer::get()->getGame(id)->dump();
			size_t slot2 = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);
			std::string slot1Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot1);
			std::string slot2Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot2);

			// Now, disable state and attempt to destroy the slot
			initGameXML();
			initConfig(false, false);
			GameContainer::reset();

			// Verify that destroyDumpSlot() doesn't do anything
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(id, slot1);
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game doesn't exist, slot doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Attempt to destroy dump slot that doesn't exist. There is no return
			// value. Just need to verify that no exceptions are found.
			GameContainer::get()->destroyDumpSlot(0, 0);
			CHECK(true);

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists but is invalid, slot doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + '0';

			// Create an invalid dumped game
			STD_FILESYSTEM::create_directory(gamePath);

			GameContainer::get()->destroyDumpSlot(0, 0);

			// Verify that the invalid game wasn't removed.
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists and is valid, slot doesn't exist") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator +
				std::to_string(id);

			// Verify that destroyDumpSlot() doesn't do anything when the slot doesn't exist
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDumpSlot(id, slot + 1);
			CHECK(STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists but is invalid, slot exists but is invalid, no slot remains") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + "0";
			std::string slotPath = gamePath + STD_FILESYSTEM::path::preferred_separator + "0";

			// Both the game and slot directories are empty, making them existing but invalid
			STD_FILESYSTEM::create_directory(statePath);
			STD_FILESYSTEM::create_directory(gamePath);
			STD_FILESYSTEM::create_directory(slotPath);

			// Verify that destroyDumpSlot() removes the entire game dump, since the only slot
			// existing was destroyed. The fact that the slot and game are invalid shouldn't
			// matter.
			CHECK(STD_FILESYSTEM::exists(slotPath));
			GameContainer::get()->destroyDumpSlot(0, 0);
			CHECK(!STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists and is valid, slot exists but is invalid, no slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			std::string newSlotPath = gamePath +
				STD_FILESYSTEM::path::preferred_separator + "1";

			// Remove the valid dump slot and replace it with one that's invalid
			STD_FILESYSTEM::remove_all(gamePath + STD_FILESYSTEM::path::preferred_separator + std::to_string(slot));
			STD_FILESYSTEM::create_directory(newSlotPath);

			// Verify that destroyDumpSlot() removes the invalid slot, along with the whole
			// dumped game since it was the only slot left
			CHECK(STD_FILESYSTEM::exists(newSlotPath));
			GameContainer::get()->destroyDumpSlot(id, 1);
			CHECK(!STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists but is invalid, slot exists but is invalid, second slot remains") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			std::string gamePath = statePath + STD_FILESYSTEM::path::preferred_separator + "0";
			std::string slot1Path = gamePath + STD_FILESYSTEM::path::preferred_separator + "0";
			std::string slot2Path = gamePath + STD_FILESYSTEM::path::preferred_separator + "1";

			// Both the game and slot directories are empty, making them existing but invalid
			STD_FILESYSTEM::create_directory(statePath);
			STD_FILESYSTEM::create_directory(gamePath);
			STD_FILESYSTEM::create_directory(slot1Path);
			STD_FILESYSTEM::create_directory(slot2Path);

			// Verify that destroyDumpSlot() only removes one slot and preserves the other
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(0, 0);
			CHECK(STD_FILESYSTEM::exists(gamePath));
			CHECK(!STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists and is valid, slot exists but is invalid, second slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot1 = GameContainer::get()->getGame(id)->dump();
			size_t slot2 = slot1 + 1;

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);
			std::string slot1Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot1);
			std::string slot2Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot2);

			// Create a second invalid dump slot
			STD_FILESYSTEM::create_directory(slot2Path);

			// Verify that only the second dump slot is destroyed
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(id, slot2);
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(!STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists but is invalid, slot exists and is valid, no slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			// Removing the dumped game's meta file makes the game invalid
			STD_FILESYSTEM::remove(gamePath + STD_FILESYSTEM::path::preferred_separator + "meta");

			// Verify that destroyDumpSlot() will remove the slot even if the dumped game is invalid,
			// along with the whole dumped game since the only slot was removed.
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDumpSlot(id, slot);
			CHECK(!STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists and is valid, slot exists and is valid, no slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);

			// Verify that destroyDumpSlot() will remove the slot even if the dumped game is invalid,
			// along with the whole dumped game since the only slot was removed.
			CHECK(STD_FILESYSTEM::exists(gamePath));
			GameContainer::get()->destroyDumpSlot(id, slot);
			CHECK(!STD_FILESYSTEM::exists(gamePath));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists but is invalid, slot exists and is valid, second slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot1 = GameContainer::get()->getGame(id)->dump();
			size_t slot2 = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);
			std::string slot1Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot1);
			std::string slot2Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot2);

			// Removing the dumped game's meta file makes the game invalid
			STD_FILESYSTEM::remove(gamePath + STD_FILESYSTEM::path::preferred_separator + "meta");

			// Verify that destroyDumpSlot() only removes the one slot, leaving the rest of the
			// invalid dump intact
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(id, slot1);
			CHECK(!STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}

		SUBCASE("State enabled, dumped game exists and is valid, slot exists and is valid, second slot remains") {

			std::string gameName = "My Game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			// Temporarily enable state so we can dump a game
			initGameXML();
			initConfig(false, true, statePath);
			GameContainer::reset();

			// Create and dump a game
			size_t id = GameContainer::get()->createGame(definition, gameName);
			size_t slot1 = GameContainer::get()->getGame(id)->dump();
			size_t slot2 = GameContainer::get()->getGame(id)->dump();

			std::string gamePath = statePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(id);
			std::string slot1Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot1);
			std::string slot2Path = gamePath +
				STD_FILESYSTEM::path::preferred_separator + std::to_string(slot2);

			// Verify that destroyDumpSlot() only removes the one slot, leaving the rest of the
			// dump intact
			CHECK(STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));
			GameContainer::get()->destroyDumpSlot(id, slot1);
			CHECK(!STD_FILESYSTEM::exists(slot1Path));
			CHECK(STD_FILESYSTEM::exists(slot2Path));

			// Restore the default configuration
			destroyGameXML();
			destroyConfig();
			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(statePath);
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getDumpedGameSlots()") {

		// TODO
	}
}
