#include <doctest.h>

#include "config.h"
#include "../include/gamecontainer.h"


// Number of ms between clock ticks (make this value small enough to ensure
// time-dependent unit tests finish quickly, but not so small that the Timer
// becomes unstable.)
constexpr size_t tickInterval = 5;


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

		std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
		std::ofstream iniFile(iniFilename, std::ofstream::out);

		iniFile << "[state]\nenabled=true\nsave_path=""\n\n" << std::endl;
		iniFile.close();

		Config::get()->load(iniFilename);

		GameContainer::reset();

		try {
			GameContainer::get();
			FAIL("GameContainer construction must fail when state is enabled and the configured state path is blank.");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}

		// Restore the default configuration
		STD_FILESYSTEM::remove(iniFilename);
		initIniFile(iniFilename, {{}});
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): Constructor: state enabled, but configured state path doesn't exist") {

		// For this particular test, this path won't actually exist
		std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
			STD_FILESYSTEM::path::preferred_separator + "/trogstate";

		std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
		std::ofstream iniFile(iniFilename, std::ofstream::out);

		iniFile << "[state]\nenabled=true\nsave_path=" << statePath
			<< "\n\n" << std::endl;
		iniFile.close();

		Config::get()->load(iniFilename);

		GameContainer::reset();

		try {
			GameContainer::get();
			FAIL("GameContainer construction must fail when state is enabled and the configured state path doesn't exist.");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}

		// Restore the default configuration
		STD_FILESYSTEM::remove(iniFilename);
		initIniFile(iniFilename, {{}});
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

		std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
		std::ofstream iniFile(iniFilename, std::ofstream::out);

		iniFile << "[state]\nenabled=true\nsave_path=" << statePath
			<< "\n\n" << std::endl;
		iniFile.close();

		Config::get()->load(iniFilename);

		GameContainer::reset();

		try {
			GameContainer::get();
			FAIL("GameContainer construction must fail when state is enabled and the configured state path is read-only.");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}

		// Restore the default configuration
		STD_FILESYSTEM::permissions(statePath, STD_FILESYSTEM::perms::owner_write);
		STD_FILESYSTEM::remove_all(statePath);
		STD_FILESYSTEM::remove(iniFilename);
		initIniFile(iniFilename, {{}});
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

			// Setup ini config options
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			// Reset GameContainer to simulate trogdord startup
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
			STD_FILESYSTEM::remove_all(statePath);
			STD_FILESYSTEM::remove(iniFilename);
			initIniFile(iniFilename, {{}});

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

			// Setup ini config options
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

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
			STD_FILESYSTEM::remove_all(statePath);
			STD_FILESYSTEM::remove(iniFilename);
			initIniFile(iniFilename, {{}});

		#endif
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): dump() and restore()") {

		SUBCASE("State disabled, zero games") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=false\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);
			GameContainer::reset();

			// Methods should just return without actually doing anything
			GameContainer::get()->dump();
			GameContainer::reset();
			GameContainer::get()->restore();

			CHECK(0 == GameContainer::get()->getGames().size());

			// Restore the default configuration
			STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=false\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled, zero games") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);
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
			STD_FILESYSTEM::remove_all(statePath);
			STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				std::ofstream iniFile2(iniFilename, std::ofstream::out);

				iniFile2 << "[state]\nenabled=false\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	// TODO: Need to write more unit tests for GameContainer::restore() that
	// will check to make sure restored games are properly indexed

	TEST_CASE("GameContainer (gamecontainer.cpp): dumpGame()") {

		// TODO
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): restoreGame()") {

		// TODO: one unit test needs to make sure replacing an existing game
		// as well as restoring a new game will properly update indexes
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): getDumpedGameIds()") {

		SUBCASE("State feature is disabled, no dumped games") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=false\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);
			GameContainer::reset();

			CHECK(0 == GameContainer::get()->getDumpedGameIds().size());

			// Restore the default configuration
			STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				std::ofstream iniFile2(iniFilename, std::ofstream::out);

				iniFile2 << "[state]\nenabled=false\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				CHECK(0 == GameContainer::get()->getDumpedGameIds().size());

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State feature is enabled, no dumped games") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);
			GameContainer::reset();

			// Since we didn't dump any games, we shouldn't get any ids back
			CHECK(0 == GameContainer::get()->getDumpedGameIds().size());

			// Restore the default configuration
			STD_FILESYSTEM::remove_all(statePath);
			STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameContainer (gamecontainer.cpp): isDumpedGameId()") {

		SUBCASE("State feature is disabled, dumped game doesn't exist") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=false\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);
			GameContainer::reset();

			// Should always return false when state is disabled
			CHECK(false == GameContainer::get()->isDumpedGameId(0));

			// Restore the default configuration
			STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				std::ofstream iniFile2(iniFilename, std::ofstream::out);

				iniFile2 << "[state]\nenabled=false\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				// Method always returns false when state feature is disabled
				CHECK(false == GameContainer::get()->isDumpedGameId(id));

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
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

				// This should now return true
				CHECK(true == GameContainer::get()->isDumpedGameId(id));

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}
}
