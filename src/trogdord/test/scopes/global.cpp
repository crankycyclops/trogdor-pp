#include <doctest.h>
#include <trogdor/version.h>

#include "../config.h"

#include <trogdord/json.h>
#include <trogdord/request.h>
#include <trogdord/response.h>

#include <trogdord/version.h>
#include <trogdord/scopes/global.h>

#include <trogdord/gamecontainer.h>


TEST_SUITE("GlobalController (scopes/global.cpp)") {

	TEST_CASE("GlobalController (scopes/global.cpp): config()") {

		std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
		initIniFile(iniFilename, {{}});

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", "get", request.GetAllocator());
		request.AddMember("scope", "global", request.GetAllocator());
		request.AddMember("action", "config", request.GetAllocator());

		rapidjson::Document response = GlobalController::get()->config(request);

		// Make sure strings were encoded properly
		CHECK(trogdor::isAscii(JSON::serialize(response)));

		// Make sure response was successful
		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("config"));
		CHECK(response["config"].IsObject());

		for (auto setting = response["config"].MemberBegin(); setting != response["config"].MemberEnd(); setting++) {

			// Make sure hidden settings don't appear in the output
			CHECK(!Config::hidden.find(setting->name.GetString())->second);

			// Make sure the output type is correct
			if (typeid(int) == *Config::types.find(setting->name.GetString())->second) {
				CHECK(response["config"][setting->name.GetString()].IsInt());
			} else if (typeid(bool) == *Config::types.find(setting->name.GetString())->second) {
				CHECK(response["config"][setting->name.GetString()].IsBool());
			} else {
				CHECK(response["config"][setting->name.GetString()].IsString());
			}
		}
	}

	// TODO: need to add a separate test case for number of games
	TEST_CASE("GlobalController (scopes/global.cpp): statistics()") {

		std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
		initIniFile(iniFilename, {{}});

		rapidjson::Document request(rapidjson::kObjectType);

		request.AddMember("method", "get", request.GetAllocator());
		request.AddMember("scope", "global", request.GetAllocator());
		request.AddMember("action", "statistics", request.GetAllocator());

		rapidjson::Document response = GlobalController::get()->statistics(request);

		CHECK(response.HasMember("status"));
		CHECK(response["status"].IsUint());
		CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

		CHECK(response.HasMember("players"));
		CHECK(response["status"].IsUint());
		CHECK(0 == response["players"].GetUint());

		CHECK(response.HasMember("version"));
		CHECK(response["version"].IsObject());

		CHECK(response["version"].HasMember("major"));
		CHECK(response["version"]["major"].IsUint());
		CHECK(TROGDORD_VERSION_MAJOR == response["version"]["major"].GetUint());

		CHECK(response["version"].HasMember("minor"));
		CHECK(response["version"]["minor"].IsUint());
		CHECK(TROGDORD_VERSION_MINOR == response["version"]["minor"].GetUint());

		CHECK(response["version"].HasMember("patch"));
		CHECK(response["version"]["patch"].IsUint());
		CHECK(TROGDORD_VERSION_PATCH == response["version"]["patch"].GetUint());

		CHECK(response.HasMember("lib_version"));
		CHECK(response["lib_version"].IsObject());

		CHECK(response["lib_version"].HasMember("major"));
		CHECK(response["lib_version"]["major"].IsUint());
		CHECK(TROGDOR_VERSION_MAJOR == response["lib_version"]["major"].GetUint());

		CHECK(response["lib_version"].HasMember("minor"));
		CHECK(response["lib_version"]["minor"].IsUint());
		CHECK(TROGDOR_VERSION_MINOR == response["lib_version"]["minor"].GetUint());

		CHECK(response["lib_version"].HasMember("patch"));
		CHECK(response["lib_version"]["patch"].IsUint());
		CHECK(TROGDOR_VERSION_PATCH == response["lib_version"]["patch"].GetUint());
	}

	TEST_CASE("GlobalController (scopes/global.cpp): dump()") {

		SUBCASE("State disabled, no games") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=false\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);
			GameContainer::reset();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "global", request.GetAllocator());
			request.AddMember("action", "dump", request.GetAllocator());

			rapidjson::Document response = GlobalController::get()->dump(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				iniFile << "[state]\nenabled=false\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();
				GameContainer::get()->createGame(definition, gameName);

				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "dump", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->dump(request);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

				// Restore the default configuration
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled, no games") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			// Setup ini file
			iniFile << "[state]\nenabled=true\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);
			GameContainer::reset();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "global", request.GetAllocator());
			request.AddMember("action", "dump", request.GetAllocator());

			rapidjson::Document response = GlobalController::get()->dump(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

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

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				// Setup ini file
				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();
				GameContainer::get()->createGame(definition, gameName);

				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "dump", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->dump(request);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}

	TEST_CASE("GlobalController (scopes/global.cpp): restore()") {

		SUBCASE("State disabled, no current games, no dumped games") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=false\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			Config::get()->load(iniFilename);
			GameContainer::reset();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "global", request.GetAllocator());
			request.AddMember("action", "restore", request.GetAllocator());

			rapidjson::Document response = GlobalController::get()->restore(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());
			CHECK(0 == GameContainer::get()->getGames().size());

			// Restore the default configuration
			STD_FILESYSTEM::remove_all(statePath);
			STD_FILESYSTEM::remove(iniFilename);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State disabled, no current games, one dumped game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				////////////////////////////////////////////
				// Step 1: Dump a game with state enabled //
				////////////////////////////////////////////

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				// Setup ini file
				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				GameContainer::get()->createGame(definition, gameName);
				GameContainer::get()->dump();

				//////////////////////////////////////////////////////////
				// Step 2: Recofigure with state disabled and show that //
				// nothing is restored and that we get the proper       //
				// status returned in the response.                     //
				//////////////////////////////////////////////////////////

				std::ofstream iniFile2(iniFilename, std::ofstream::out);
				iniFile2 << "[state]\nenabled=false\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "restore", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->restore(request);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

				// Verify that the previously dumped game isn't restored
				// since state is now disabled.
				CHECK(0 == GameContainer::get()->getGames().size());

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State disabled, one current games, no dumped games") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				///////////////////////////
				// Step 1: Create a game //
				///////////////////////////

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				// Setup ini file
				iniFile << "[state]\nenabled=false\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				GameContainer::get()->createGame(definition, gameName);
				CHECK(1 == GameContainer::get()->getGames().size());

				/////////////////////////////////////////////////
				// Step 2: With state disabled, call restore() //
				/////////////////////////////////////////////////

				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "restore", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->restore(request);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

				// Verify that the current game wasn't destroyed after the
				// call to restore()
				CHECK(1 == GameContainer::get()->getGames().size());

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State disabled, one current games, one dumped game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				////////////////////////////////////////////
				// Step 1: Dump a game with state enabled //
				////////////////////////////////////////////

				std::string dumpedName = "Dumped Game";
				std::string gameName = "Current Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				// Setup ini file
				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				size_t dumpedId = GameContainer::get()->createGame(definition, dumpedName);
				GameContainer::get()->dump();

				CHECK(0 == dumpedId);
				CHECK(0 == dumpedName.compare(GameContainer::get()->getGame(dumpedId)->getName()));

				////////////////////////////////////////////////////////////////
				// Step 2: Reset server with state disabled and create a game //
				////////////////////////////////////////////////////////////////

				std::ofstream iniFile2(iniFilename, std::ofstream::out);
				iniFile2 << "[state]\nenabled=false\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				size_t gameId = GameContainer::get()->createGame(definition, gameName);

				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "restore", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->restore(request);

				// Verify that the call failed and that the new game we
				// created continues to exist and that the dumped one doesn't.
				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_UNSUPPORTED == response["status"].GetUint());

				CHECK(1 == GameContainer::get()->getGames().size());
				CHECK(gameId == dumpedId);
				CHECK(0 == gameName.compare(GameContainer::get()->getGame(gameId)->getName()));

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled, no current games, no dumped games") {

			std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "/trogstate";

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=true\nsave_path=" << statePath
				<< "\n\n" << std::endl;
			iniFile.close();

			// Make a read-only state directory
			STD_FILESYSTEM::create_directory(statePath);

			Config::get()->load(iniFilename);
			GameContainer::reset();

			rapidjson::Document request(rapidjson::kObjectType);

			request.AddMember("method", "post", request.GetAllocator());
			request.AddMember("scope", "global", request.GetAllocator());
			request.AddMember("action", "restore", request.GetAllocator());

			rapidjson::Document response = GlobalController::get()->restore(request);

			CHECK(response.HasMember("status"));
			CHECK(response["status"].IsUint());
			CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());
			CHECK(0 == GameContainer::get()->getGames().size());

			// Restore the default configuration
			STD_FILESYSTEM::remove_all(statePath);
			STD_FILESYSTEM::remove(iniFilename);
			initIniFile(iniFilename, {{}});
		}

		SUBCASE("State enabled, no current games, one dumped game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				////////////////////////////////////////////
				// Step 1: Dump a game with state enabled //
				////////////////////////////////////////////

				std::string dumpedName = "Dumped Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				// Setup ini file
				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				GameContainer::get()->createGame(definition, dumpedName);
				GameContainer::get()->dump();

				CHECK(1 == GameContainer::get()->getGames().size());

				//////////////////////////////////////
				// Step 1: Reset server and restore //
				//////////////////////////////////////

				std::ofstream iniFile2(iniFilename, std::ofstream::out);
				iniFile2 << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				// Before restoring, number of games should be 0
				CHECK(0 == GameContainer::get()->getGames().size());

				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "restore", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->restore(request);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				// Now, there should be one game again after restoring the
				// previous dump
				CHECK(1 == GameContainer::get()->getGames().size());

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled, one current games, no dumped games") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				std::string gameName = "My Game";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				// Setup ini file
				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				// Start off with 0 games
				CHECK(0 == GameContainer::get()->getGames().size());

				// Create a game
				GameContainer::get()->createGame(definition, gameName);
				CHECK(1 == GameContainer::get()->getGames().size());

				// Call restore with 0 dumped games and verify that it leaves
				// the currently created game intact.
				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "restore", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->restore(request);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				CHECK(1 == GameContainer::get()->getGames().size());

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}

		SUBCASE("State enabled, one current game, one dumped game") {

			#ifndef CORE_UNIT_TEST_DEFINITION_FILE

				FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

			#else

				///////////////////////////////////////////////
				// Step 1: Dump a game and then reset server //
				///////////////////////////////////////////////

				std::string dumpedName = "Dumped Game";
				std::string gameName = "Game Name";
				std::string definition = CORE_UNIT_TEST_DEFINITION_FILE;
				std::string statePath = STD_FILESYSTEM::temp_directory_path().string() +
					STD_FILESYSTEM::path::preferred_separator + "/trogstate";

				std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
				std::ofstream iniFile(iniFilename, std::ofstream::out);

				// Make a read-only state directory
				STD_FILESYSTEM::create_directory(statePath);

				// Setup ini file
				iniFile << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				GameContainer::get()->createGame(definition, dumpedName);
				GameContainer::get()->dump();

				//////////////////////////////////////
				// Step 2: Reset server and restore //
				//////////////////////////////////////

				std::ofstream iniFile2(iniFilename, std::ofstream::out);
				iniFile2 << "[state]\nenabled=true\nsave_path=" << statePath
					<< "\n\n" << std::endl;
				iniFile2.close();

				Config::get()->load(iniFilename);
				GameContainer::reset();

				GameContainer::get()->createGame(definition, gameName);

				// We should have one existing game before restoring
				CHECK(1 == GameContainer::get()->getGames().size());

				rapidjson::Document request(rapidjson::kObjectType);

				request.AddMember("method", "post", request.GetAllocator());
				request.AddMember("scope", "global", request.GetAllocator());
				request.AddMember("action", "restore", request.GetAllocator());

				rapidjson::Document response = GlobalController::get()->restore(request);

				CHECK(response.HasMember("status"));
				CHECK(response["status"].IsUint());
				CHECK(Response::STATUS_SUCCESS == response["status"].GetUint());

				// Now, there should be two games
				CHECK(2 == GameContainer::get()->getGames().size());

				// Restore the default configuration
				STD_FILESYSTEM::remove_all(statePath);
				STD_FILESYSTEM::remove(iniFilename);
				initIniFile(iniFilename, {{}});

			#endif
		}
	}
}
