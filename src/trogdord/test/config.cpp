#include <doctest.h>


#include "config.h"


TEST_SUITE("Config (config.cpp)") {

	TEST_CASE("Config (config.cpp): Construction without an ini file") {

		// I can't really construct this outside Config::get(), but I can
		// reset it by loading config values from an empty path.
		Config::get()->load("");

		// Make sure all config values are set to their defaults
		for (auto const &value: Config::get()->DEFAULTS) {
			CHECK(0 == value.second.compare(Config::get()->getString(value.first)));
		}
	}

	TEST_CASE("Config (config.cpp): Construction with an invalid ini file") {

		SUBCASE("ini file path points to file that doesn't exist") {

			Config::get()->load("/path/to/file/that/doesnt/exist");

			// Test to make sure that the config values are all set to their
			// defaults
			for (auto const &value: Config::get()->DEFAULTS) {
				CHECK(0 == value.second.compare(Config::get()->getString(value.first)));
			}
		}

		SUBCASE("ini file path is a directory") {

			// We know for a fact that this path exists and that it's a
			// directory.
			Config::get()->load(STD_FILESYSTEM::temp_directory_path());

			// Once again, test to make sure the config values are set to
			// their defaults
			for (auto const &value: Config::get()->DEFAULTS) {
				CHECK(0 == value.second.compare(Config::get()->getString(value.first)));
			}
		}
	}

	TEST_CASE ("Config (config.cpp): Construction with valid ini file") {

		std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";

		// We're going to try outputting an ini file with each option
		// individually changed from its default and verify that only that
		// option gets changed from the defaults when the ini file is loaded.
		for (auto const &value: Config::get()->DEFAULTS) {

			std::ofstream iniFile (iniFilename, std::ofstream::out);

			iniFile << '[' << value.first.substr(0, value.first.find('.')) << ']' << std::endl;
			iniFile << value.first.substr(value.first.find('.') + 1) << "=aDifferentValue" << std::endl;
			iniFile.close();

			Config::get()->load(iniFilename);

			for (auto const &iniValue: Config::get()->DEFAULTS) {
				if (0 == iniValue.first.compare(value.first)) {
					CHECK(0 == Config::get()->getString(iniValue.first).compare("aDifferentValue"));
				} else {
					CHECK(0 == Config::get()->getString(iniValue.first).compare(iniValue.second));
				}
			}
		}

		STD_FILESYSTEM::remove(iniFilename);
	}

	TEST_CASE ("Config (config.cpp): getBool()") {

		SUBCASE("All possible values for true") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "1"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "true"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "True"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "TRUE"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "tRue"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "trUe"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "truE"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "TRue"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "TrUe"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "TruE"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "tRUe"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "tRuE"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "trUE"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "TRUe"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "TRuE"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "tRUE"}});
			CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);
		}

		SUBCASE("All possible values for false") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "0"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "false"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "False"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FALSE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "fAlse"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "faLse"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "falSe"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "falsE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FAlse"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FaLse"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FalSe"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FalsE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "fALse"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "fAlSe"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "fAlsE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "faLSe"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "falSE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FALse"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FAlSe"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "FAlsE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "fALSe"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "fALsE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);

			initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, "faLSE"}});
			CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
			STD_FILESYSTEM::remove(iniFilename);
		}

		SUBCASE("Invalid value") {

			// TODO
		}
	}
}
