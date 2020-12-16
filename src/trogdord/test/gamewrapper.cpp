#include <doctest.h>
#include <cstdlib>

#include <trogdor/entities/player.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include "config.h"

#include "../include/gamewrapper.h"
#include "../include/filesystem.h"
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
				std::ofstream iniFile (iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=false\nsave_path=" << statePath << "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);

				try {

					GameWrapper test(0, definition, name);
					test.dump();

					for (const auto &subdir: STD_FILESYSTEM::directory_iterator(statePath)) {
						FAIL(subdir.path().filename().string() + ": No data should be dumped when state id disabled");
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
				std::ofstream iniFile (iniFilename, std::ofstream::out);

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

		SUBCASE("Config::CONFIG_KEY_MAX_DUMPS_PER_GAME = 0") {

			// TODO
		}

		SUBCASE("Config::CONFIG_KEY_MAX_DUMPS_PER_GAME = 1") {

			// TODO
		}

		SUBCASE("Config::CONFIG_KEY_MAX_DUMPS_PER_GAME = 2") {

			// TODO
		}
	}

	TEST_CASE("GameWrapper (gamewrapper.cpp): dumpDestroy()") {

		// TODO
	}
}
