#include <doctest.h>
#include <cstdlib>

#include <trogdor/entities/player.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include "config.h"

#include "../include/gamewrapper.h"
#include "../include/filesystem.h"
#include "../include/serial/drivermap.h"

#include "../include/exception/serverexception.h"
#include "../include/exception/serialdrivernotfound.h"


TEST_SUITE("GameWrapper (gamewrapper.cpp)") {

	TEST_CASE("GameWrapper (gamewrapper.cpp): Construction of game with an invalid definition path") {

		std::string name = "I'm a game";
		std::string definition = "/not/a/valid/definition.xml";

		// Try without metadata
		try {
			GameWrapper test(0, definition, name);
			FAIL("Construction of GameWrapper should not succeed with an invalid definition path");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}

		// Try with metadata
		try {
			GameWrapper test(0, definition, name, {{"key", "value"}});
			FAIL("Construction of GameWrapper should not succeed with an invalid definition path");
		}

		catch (const ServerException &e) {
			CHECK(true);
		}
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): Construction of a new game without metadata") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string name = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			try {
				GameWrapper test(0, definition, name);
				CHECK(test.get() != nullptr);
			}

			catch (const ServerException &e) {
				FAIL("GameWrapper constructor shouldn't fail when the definition file path is valid");
			}

		#endif
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): Construction of a new game with metadata") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string name = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			try {
				GameWrapper test(0, definition, name, {{"key", "value"}});
				CHECK(test.get() != nullptr);
				CHECK(0 == test.get()->getMeta("key").compare("value"));
			}

			catch (const ServerException &e) {
				FAIL("GameWrapper constructor shouldn't fail when the definition file path is valid");
			}

		#endif
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): getId(), getName(), getDefinition() and getCreated()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string name = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			try {

				GameWrapper test(0, definition, name);

				CHECK(test.get() != nullptr);
				CHECK(0 == test.getId());
				CHECK(0 == test.getName().compare(name));
				CHECK(0 == test.getDefinition().compare(STD_FILESYSTEM::path(definition).filename()));

				// Every once in a blue moon, there's a little bit of lag and
				// the things are off by about 1. I'm okay with that.
				CHECK(abs(std::difftime(std::time(nullptr), test.getCreated())) <= 1);
			}

			catch (const ServerException &e) {
				FAIL("GameWrapper constructor shouldn't fail when the definition file path is valid");
			}

		#endif
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): getNumPlayers()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			std::string name = "I'm a game";
			std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;

			try {

				GameWrapper test(0, definition, name);

				// Check number of players before and after adding one to
				// the game
				CHECK(0 == test.getNumPlayers());

				std::shared_ptr<trogdor::entity::Player> player = test.get()->createPlayer(
					"player",
					std::make_unique<trogdor::NullOut>(),
					std::make_unique<trogdor::NullErr>()
				);

				test.get()->insertPlayer(player);
				CHECK(1 == test.getNumPlayers());
				test.get()->removePlayer(player->getName());
				CHECK(0 == test.getNumPlayers());
			}

			catch (const ServerException &e) {
				FAIL("GameWrapper constructor shouldn't fail when the definition file path is valid");
			}

		#endif
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): dump()") {

		SUBCASE("State is disabled") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=false\nsave_path=" << statePath << "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					GameWrapper test(0, definition, name);
					test.dump();

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(statePath)) {
						FAIL(subdir.path().filename().string() + ": No data should be dumped when state is disabled");
					}
				}

				catch (const ServerException &e) {
					FAIL("GameWrapper constructor failed or there was an error calling GameWrapper::dump()");
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("Invalid serialization driver") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nformat=invalid\nsave_path=" << statePath << "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					GameWrapper test(0, definition, name);
					test.dump();

					FAIL("GameWrapper::dump() should fail if serialization driver isn't valid");
				}

				catch (const SerialDriverNotFound &e) {
					CHECK(true);
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		// Verify that we get an unlimited number of dumps per game
		SUBCASE("Config::CONFIG_KEY_MAX_DUMPS_PER_GAME = 0") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				const int numDumps = 10;
				const size_t gameId = 0;

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nmax_dumps_per_game=0\nsave_path=" << statePath << "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					GameWrapper test(gameId, definition, name);

					// Helps us verify the auto-incrementing id
					std::set<size_t> validDumpInts;

					for (int i = 0; i < numDumps; i++) {
						test.dump();
						validDumpInts.insert(i);
					}

					int numActuallyDumped = 0;

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(gameStatePath)) {
						numActuallyDumped++;
						CHECK(validDumpInts.find(std::stoi(subdir.path().filename().string())) != validDumpInts.end());
					}

					CHECK(numActuallyDumped == numDumps);
				}

				catch (const SerialDriverNotFound &e) {
					FAIL(e.what());
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		// Verify that we get a maximum of 1 dump per game
		SUBCASE("Config::CONFIG_KEY_MAX_DUMPS_PER_GAME = 1") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				const int numDumps = 10;
				const size_t gameId = 0;

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nmax_dumps_per_game=1\nsave_path=" << statePath << "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					GameWrapper test(gameId, definition, name);

					// Helps us verify the auto-incrementing id
					size_t validDumpInt = 9;

					for (int i = 0; i < numDumps; i++) {
						test.dump();
					}

					int numActuallyDumped = 0;

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(gameStatePath)) {
						numActuallyDumped++;
						CHECK(validDumpInt == std::stoi(subdir.path().filename().string()));
					}

					CHECK(1 == numActuallyDumped);
				}

				catch (const SerialDriverNotFound &e) {
					FAIL(e.what());
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		// Verify that we get a maximum of 2 dumps per game
		SUBCASE("Config::CONFIG_KEY_MAX_DUMPS_PER_GAME = 2") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				const int numDumps = 10;
				const size_t gameId = 0;

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nmax_dumps_per_game=2\nsave_path=" << statePath << "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					GameWrapper test(gameId, definition, name);

					// Helps us verify the auto-incrementing id
					std::set<size_t> validDumpInts;

					validDumpInts.insert(numDumps - 1);
					validDumpInts.insert(numDumps - 2);

					for (int i = 0; i < numDumps; i++) {
						test.dump();
					}

					int numActuallyDumped = 0;

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(gameStatePath)) {
						numActuallyDumped++;
						CHECK(validDumpInts.find(std::stoi(subdir.path().filename().string())) != validDumpInts.end());
					}

					CHECK(2 == numActuallyDumped);
				}

				catch (const SerialDriverNotFound &e) {
					FAIL(e.what());
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("Verify validity of serialized data") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				const size_t gameId = 0;

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nformat=json\nmax_dumps_per_game=1\nsave_path="
					<< statePath << "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					GameWrapper test(gameId, definition, name);
					test.dump();

					std::string gameStateSlotPath = gameStatePath +
						STD_FILESYSTEM::path::preferred_separator + '0';

					CHECK(STD_FILESYSTEM::exists(gameStateSlotPath));

					std::string metaFilename = gameStateSlotPath +
						STD_FILESYSTEM::path::preferred_separator + "meta";
					std::string gameFilename = gameStateSlotPath +
						STD_FILESYSTEM::path::preferred_separator + "game";

					CHECK(STD_FILESYSTEM::exists(metaFilename));
					CHECK(STD_FILESYSTEM::exists(gameFilename));

					auto &json = serial::DriverMap::get("json");

					// Make sure we can deserialize the game data properly.
					// An uncaught exception will occur if deserialization
					// is unsuccessful, causing the test case to fail.
					ifstream metaFile(metaFilename);
					ifstream gameFile(gameFilename);

					ostringstream metaStr;
					ostringstream gameStr;

					metaStr << metaFile.rdbuf();
					gameStr << gameFile.rdbuf();

					std::shared_ptr<trogdor::serial::Serializable> metaData = json->deserialize(metaStr.str());
					std::shared_ptr<trogdor::serial::Serializable> gameData = json->deserialize(gameStr.str());
				}

				catch (const SerialDriverNotFound &e) {
					FAIL(e.what());
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): dumpDestroy()") {

		SUBCASE("State is disabled") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				// Step 1: enable dumps and create one
				const size_t gameId = 0;

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";
				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				std::string gameStateSlotPath = gameStatePath +
					STD_FILESYSTEM::path::preferred_separator + '0';

				try {

					GameWrapper test(gameId, definition, name);
					test.dump();

					CHECK(STD_FILESYSTEM::exists(gameStateSlotPath));
				}

				catch (const SerialDriverNotFound &e) {
					FAIL(e.what());
				}

				// Step 2: disable dumps and verify that calling destroyDump()
				// doesn't do anything (the game we just dumped should remain
				// intact.)
				std::ofstream iniFile2(iniFilename, std::ofstream::out);
				iniFile2 << "[state]\nenabled=false\nsave_path=" << statePath << "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);

				try {

					// This creates another game with the same id. I can't
					// deserialize the dump, because that would require me
					// to enable state. Nevertheless, this should result in
					// destroyDump() getting invoked with the proper id.
					GameWrapper test(0, definition, name);
					test.destroyDump();

					CHECK(STD_FILESYSTEM::exists(gameStateSlotPath));
					CHECK(STD_FILESYSTEM::exists(gameStateSlotPath + STD_FILESYSTEM::path::preferred_separator + "meta"));
					CHECK(STD_FILESYSTEM::exists(gameStateSlotPath + STD_FILESYSTEM::path::preferred_separator + "game"));
				}

				catch (const ServerException &e) {
					FAIL("GameWrapper constructor failed or there was an error calling GameWrapper::dump()");
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("Root state path doesn't exist") {
			// TODO: verify that nothing happens (no exceptions, no return value)
		}

		SUBCASE("Game id's root dump path doesn't exist") {
			// TODO: verify that nothing happens (no exceptions, no return value)
		}

		SUBCASE("Game dump exists") {
			// TODO: verify we can remove it
		}
	}
}
