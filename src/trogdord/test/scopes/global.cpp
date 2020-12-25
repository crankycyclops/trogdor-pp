#include <doctest.h>
#include <trogdor/version.h>

#include "../config.h"

#include "../../include/json.h"
#include "../../include/request.h"
#include "../../include/response.h"

#include "../../include/version.h"
#include "../../include/scopes/global.h"

#include "../../include/gamecontainer.h"


TEST_SUITE("GlobalController (scopes/global.cpp)") {

	// TODO: need to add a separate test case for number of games
	TEST_CASE("GlobalController (scopes/global.cpp): resolve() with no action and no default action") {

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

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::ofstream iniFile(iniFilename, std::ofstream::out);

			iniFile << "[state]\nenabled=false\n\n" << std::endl;
			iniFile.close();

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

			// Restore the default configuration
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
		}

		SUBCASE("State disabled, one current games, one dumped game") {
			// TODO: should return 501 unsupported, one game should remain, and it should be the same
		}

		SUBCASE("State enabled, no current games, no dumped games") {
			// TODO: should return 200, no games should exist
		}

		SUBCASE("State enabled, no current games, one dumped game") {
			// TODO: should return 200, have the one previously dumped game
		}

		SUBCASE("State enabled, one current games, no dumped games") {
			// TODO: should return 200, zero games should remain
		}

		SUBCASE("State enabled, one current games, one dumped game") {
			// TODO: should return 200, game should be replaced by previously dumped one
		}
	}
}
