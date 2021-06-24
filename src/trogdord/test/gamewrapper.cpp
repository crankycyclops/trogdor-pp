#include <doctest.h>
#include <cstdlib>

#include <trogdor/entities/player.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include "config.h"

#include <trogdord/gamewrapper.h>
#include <trogdord/filesystem.h>
#include <trogdord/serial/drivermap.h>

#include <trogdord/exception/serverexception.h>
#include <trogdord/exception/unsupportedoperation.h>
#include <trogdord/exception/serialdrivernotfound.h>
#include <trogdord/exception/gamenotfound.h>
#include <trogdord/exception/gameslotnotfound.h>


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
					size_t slot = test.dump();

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(statePath)) {
						std::string message = subdir.path().filename().string() + ": No data should be dumped when state is disabled";
						FAIL(message);
					}

					// dump() always returns 0 without effect when state is
					// disabled.
					CHECK(0 == slot);
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
						validDumpInts.insert(test.dump());
					}

					int numActuallyDumped = 0;

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(gameStatePath)) {

						// Skip over "meta", the only valid file within the game id's directory
						if (
							STD_FILESYSTEM::is_regular_file(subdir.path()) &&
							0 == subdir.path().filename().string().compare("meta")
						) {
							continue;
						}

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
						size_t slot = test.dump();
						CHECK(i == slot);
					}

					int numActuallyDumped = 0;

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(gameStatePath)) {

						// Skip over "meta", the only valid file within the game id's directory
						if (
							STD_FILESYSTEM::is_regular_file(subdir.path()) &&
							0 == subdir.path().filename().string().compare("meta")
						) {
							continue;
						}

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

					for (int i = 0; i < numDumps; i++) {
						validDumpInts.insert(test.dump());
					}

					int numActuallyDumped = 0;

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(gameStatePath)) {

						// Skip over "meta", the only valid file within the game id's directory
						if (
							STD_FILESYSTEM::is_regular_file(subdir.path()) &&
							0 == subdir.path().filename().string().compare("meta")
						) {
							continue;
						}

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
					size_t slot = test.dump();

					std::string gameStateSlotPath = gameStatePath +
						STD_FILESYSTEM::path::preferred_separator + std::to_string(slot);

					CHECK(STD_FILESYSTEM::exists(gameStateSlotPath));

					std::string timestampFilename = gameStateSlotPath +
						STD_FILESYSTEM::path::preferred_separator + "timestamp";
					std::string formatFilename = gameStateSlotPath +
						STD_FILESYSTEM::path::preferred_separator + "format";
					std::string metaFilename = gameStatePath +
						STD_FILESYSTEM::path::preferred_separator + "meta";
					std::string gameFilename = gameStateSlotPath +
						STD_FILESYSTEM::path::preferred_separator + "game";

					CHECK(STD_FILESYSTEM::exists(timestampFilename));
					CHECK(STD_FILESYSTEM::exists(formatFilename));
					CHECK(STD_FILESYSTEM::exists(metaFilename));
					CHECK(STD_FILESYSTEM::exists(gameFilename));

					std::ifstream timestampFile(timestampFilename);
					std::string timeStr(
						(std::istreambuf_iterator<char>(timestampFile)),
						std::istreambuf_iterator<char>()
					);

					// Make sure the timestamp file really contains a timestamp
					CHECK(trogdor::isValidInteger(timeStr));

					std::ifstream formatFile(formatFilename);
					std::string formatStr(
						(std::istreambuf_iterator<char>(formatFile)),
						std::istreambuf_iterator<char>()
					);

					// Make sure format was correctly recorded
					CHECK(0 == formatStr.compare("json"));

					auto &json = serial::DriverMap::get("json");

					// Make sure game meta data is validly dumped
					std::unordered_map<std::string, std::string> metaData = GameWrapper::getDumpedGameMeta(metaFilename);

					CHECK(metaData.end() != metaData.find("name"));
					CHECK(metaData.end() != metaData.find("definition"));
					CHECK(metaData.end() != metaData.find("created"));

					CHECK(0 == name.compare(metaData["name"]));
					CHECK(0 == STD_FILESYSTEM::path(definition).filename().compare(metaData["definition"]));
					CHECK(0 == name.compare(metaData["name"]));

					#if SIZE_MAX == UINT64_MAX
						size_t created = stoull(metaData["created"]);
					#else
						size_t created = stoul(metaData["created"]);
					#endif

					// Validate that this field is a valid timestamp
					CHECK(0 == std::to_string(created).compare(metaData["created"]));

					// Make sure we can deserialize the game data properly.
					// An uncaught exception will occur if deserialization
					// is unsuccessful, causing the test case to fail.
					ifstream gameFile(gameFilename);
					ostringstream gameStr;

					gameStr << gameFile.rdbuf();
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

	TEST_CASE("GameWrapper (gamewrapper.cpp) and GameContainer (gamecontainer.cpp): dumpDestroy()") {

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
					CHECK(STD_FILESYSTEM::exists(gameStatePath + STD_FILESYSTEM::path::preferred_separator + "meta"));
					CHECK(STD_FILESYSTEM::exists(gameStateSlotPath + STD_FILESYSTEM::path::preferred_separator + "timestamp"));
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

		SUBCASE("Game id's root dump path doesn't exist") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				const size_t gameId = 0;

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

				// We didn't create a dump. Verify that destroyDump() does
				// nothing and that it doesn't throw any exceptions.
				try {

					CHECK(!STD_FILESYSTEM::exists(gameStatePath));

					GameWrapper test(gameId, definition, name);
					test.destroyDump();

					CHECK(!STD_FILESYSTEM::exists(gameStatePath));
				}

				catch (const ServerException &e) {
					FAIL(e.what());
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("Game dump exists") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				const size_t gameId = 0;

				std::string name = "I'm a game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				STD_FILESYSTEM::create_directory(statePath);

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				std::string gameStatePath = statePath +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(gameId);

				try {

					// First, make sure the game path didn't already exist
					CHECK(!STD_FILESYSTEM::exists(gameStatePath));
					GameWrapper test(gameId, definition, name);

					// Next, make a dump and verify it was created
					test.dump();
					CHECK(STD_FILESYSTEM::exists(gameStatePath));

					// Finally, destroy the dump and make sure it was removed
					test.destroyDump();
					CHECK(!STD_FILESYSTEM::exists(gameStatePath));
				}

				catch (const ServerException &e) {
					FAIL(e.what());
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): deserialization constructor") {

		SUBCASE("State is disabled") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=false\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			auto gameStatePath = STD_FILESYSTEM::path(
				statePath + STD_FILESYSTEM::path::preferred_separator + '0'
			);

			try {
				GameWrapper test(gameStatePath);
				FAIL("GameWrapper construction from dumped game should never succeed when state feature is disabled.");
			}

			catch (const UnsupportedOperation &e) {
				CHECK(true);
			}

			STD_FILESYSTEM::remove(iniFilename);
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("Game dump doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath << "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			auto gameStatePath = STD_FILESYSTEM::path(
				statePath + STD_FILESYSTEM::path::preferred_separator + '0'
			);

			// We attempt to deserialize a game when the game state path
			// doesn't exist
			try {
				GameWrapper test(gameStatePath);
				FAIL("GameWrapper construction from dumped game should never succeed when the game's dump path doesn't exist.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			STD_FILESYSTEM::remove(iniFilename);
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("Game dump directory exists but is empty (no dump slots)") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath << "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			auto gameStatePath = STD_FILESYSTEM::path(
				statePath + STD_FILESYSTEM::path::preferred_separator + '0'
			);

			// Very similar to the test above, except that for this one, we
			// create an empty game directory with no dump slots
			STD_FILESYSTEM::create_directory(gameStatePath);

			try {
				GameWrapper test(gameStatePath);
				FAIL("GameWrapper construction from dumped game should never succeed when there are zero dump slots but should be treated as if the game dump doesn't exist.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			STD_FILESYSTEM::remove(iniFilename);
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("Game dump exists, but is invalid (files don't contain valid serialized data)") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath << "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			auto gameStatePath = STD_FILESYSTEM::path(
				statePath + STD_FILESYSTEM::path::preferred_separator + '0'
			);

			STD_FILESYSTEM::create_directory(gameStatePath);

			// Create dump files containing non-deserializable data
			std::string slotPath = gameStatePath.string() + STD_FILESYSTEM::path::preferred_separator + '0';
			std::string metaDataPath = gameStatePath.string() + STD_FILESYSTEM::path::preferred_separator + "meta";
			std::string timestampDataPath = slotPath + STD_FILESYSTEM::path::preferred_separator + "timestamp";
			std::string formatDataPath = slotPath + STD_FILESYSTEM::path::preferred_separator + "format";
			std::string gameDataPath = slotPath + STD_FILESYSTEM::path::preferred_separator + "game";

			STD_FILESYSTEM::create_directory(slotPath);

			fstream timestampFile(timestampDataPath, std::fstream::out);
			timestampFile << "0" << std::endl;
			timestampFile.close();

			fstream formatFile(formatDataPath, std::fstream::out);
			formatFile << "json" << std::endl;
			formatFile.close();

			fstream metaFile(metaDataPath, std::fstream::out);
			metaFile << "I'm not valid JSON!" << std::endl;
			metaFile.close();

			fstream gameFile(gameDataPath, std::fstream::out);
			gameFile << "I'm not valid JSON!" << std::endl;
			gameFile.close();

			try {
				GameWrapper test(gameStatePath);
				FAIL("GameWrapper construction from dumped game should never succeed when the dump contains invalid data.");
			}

			catch (const trogdor::UndefinedException &e) {
				CHECK(true);
			}

			STD_FILESYSTEM::remove(iniFilename);
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("Game dump is valid") {

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

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					// Step 1: create and dump a game
					GameWrapper test(gameId, definition, name);

					test.dump();
					CHECK(STD_FILESYSTEM::exists(gameStatePath));

					// Step 2: demonstrate that we can restore the game
					// using the deserialization constructor
					GameWrapper testCopy((STD_FILESYSTEM::path(gameStatePath)));

					// Just some rudimentary spot checks to make sure that
					// things really were deserialized property and that we
					// can access the underlying instance of trogdor::Game.
					CHECK(0 == name.compare(testCopy.getName()));
					CHECK(0 == STD_FILESYSTEM::path(definition).filename().string().compare(testCopy.getDefinition()));
					CHECK(0 == testCopy.get()->getPlayers().size());
				}

				catch (const ServerException &e) {
					FAIL(e.what());
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("Deserialization constructor with optional slot: dumped game doesn't exist") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			STD_FILESYSTEM::create_directory(statePath);

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath << "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			auto gameStatePath = STD_FILESYSTEM::path(
				statePath + STD_FILESYSTEM::path::preferred_separator + '0'
			);

			try {
				GameWrapper testGame(gameStatePath, 0);
				FAIL("GameWrapper construction should not succeed when dumped game doesn't exist.");
			}

			catch (const GameNotFound &e) {
				CHECK(true);
			}

			STD_FILESYSTEM::remove(iniFilename);
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("Deserialization constructor with optional slot: dumped game exists, but slot doesn't") {

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

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					// Step 1: create and dump a game (will populate slot 0)
					GameWrapper test(gameId, definition, name);

					test.dump();
					CHECK(STD_FILESYSTEM::exists(gameStatePath));

					// Step 2: attempt to restore game slot 1, which is not
					// an existing dump slot.
					GameWrapper testCopy(gameStatePath, 1);
					FAIL("GameWrapper construction should fail when passing a non-existent dump slot");
				}

				catch (const GameSlotNotFound &e) {
					CHECK(true);
				}

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("Deserialization constructor with optional slot: dumped game and slot both exist") {

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

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				// Step 1: create and dump a game (will populate slot 0)
				GameWrapper test(gameId, definition, name);

				test.dump();
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// If this fails, it'll throw an exception that doctest will
				// catch and report
				GameWrapper testCopy(gameStatePath, 0);
				CHECK(0 == testCopy.getRestoredSlot());

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("Deserialization constructor without slot: verify that the latest slot is loaded by default") {

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

				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				// Step 1: create and dump a game with no meta set
				GameWrapper test(gameId, definition, name);

				test.dump();
				CHECK(STD_FILESYSTEM::exists(gameStatePath));

				// Step 2: set a meta value, then dump again
				test.get()->setMeta("metakey", "wee");
				test.dump();

				// By default, if no slot is specified, we should restore
				// the latest version with the meta value set.
				GameWrapper testCopy(gameStatePath);
				CHECK(0 == testCopy.get()->getMeta("metakey").compare("wee"));
				CHECK(1 == testCopy.getRestoredSlot());

				// Let's also verify that the previous dump does exist
				GameWrapper testCopy2(gameStatePath, 0);
				CHECK(0 == testCopy2.get()->getMeta("metakey").compare(""));
				CHECK(0 == testCopy2.getRestoredSlot());

				STD_FILESYSTEM::remove(iniFilename);
				STD_FILESYSTEM::remove_all(statePath);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): getRestoredSlot()") {

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

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			// Step 1: create an original game (shouldn't have a value)
			GameWrapper test(gameId, definition, name);
			CHECK(std::nullopt == test.getRestoredSlot());

			size_t slot = test.dump();

			GameWrapper testCopy(gameStatePath);
			CHECK(slot == *testCopy.getRestoredSlot());

			STD_FILESYSTEM::remove(iniFilename);
			STD_FILESYSTEM::remove_all(statePath);
			initIniFile(iniFilename, {{}});

		#endif
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): getDumpedGameMeta()") {

		// TODO
	}
}
