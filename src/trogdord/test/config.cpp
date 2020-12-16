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

	TEST_CASE ("Config (config.cpp): Construction with valid ini file and getString()") {

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

	TEST_CASE ("Config (config.cpp): getUInt()") {

		SUBCASE("Valid unsigned integer") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::vector<std::string> values = {
				"0", "1", "2"
			};

			for (const auto &value: values) {
				initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, value}});
				CHECK(std::atoi(value.c_str()) == Config::get()->getInt(Config::CONFIG_KEY_PORT));
				STD_FILESYSTEM::remove(iniFilename);
			}
		}

		SUBCASE("Invalid unsigned integer") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::vector<std::string> values = {
				"blah", "true", "false", "t", "f", "1.2", "-1.2", "-1", "-2"
			};

			for (const auto &value: values) {

				initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, value}});

				try {
					Config::get()->getUInt(Config::CONFIG_KEY_PORT);
					CHECK(false);
				} catch (const ConfigInvalidValue &e) {
					CHECK(true);
				}

				STD_FILESYSTEM::remove(iniFilename);
			}
		}
	}

	TEST_CASE ("Config (config.cpp): getInt()") {

		SUBCASE("Valid integer") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::vector<std::string> values = {
				"0", "-1", "1", "5", "-5"
			};

			for (const auto &value: values) {
				initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, value}});
				CHECK(std::atoi(value.c_str()) == Config::get()->getInt(Config::CONFIG_KEY_PORT));
				STD_FILESYSTEM::remove(iniFilename);
			}
		}

		SUBCASE("Invalid integer") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::vector<std::string> values = {
				"blah", "true", "false", "t", "f", "0.1", "1.2", "-1.2"
			};

			for (const auto &value: values) {

				initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, value}});

				try {
					Config::get()->getInt(Config::CONFIG_KEY_PORT);
					CHECK(false);
				} catch (const ConfigInvalidValue &e) {
					CHECK(true);
				}

				STD_FILESYSTEM::remove(iniFilename);
			}
		}
	}

	TEST_CASE ("Config (config.cpp): getBool()") {

		SUBCASE("All possible values for true") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::vector<std::string> values = {
				"1", "t", "T", "true", "True", "TRUE", "tRue", "trUe",
				"truE", "TRue", "TrUe", "TruE", "tRUe", "tRuE", "trUE",
				"TRUe", "TRuE", "tRUE"
			};

			for (const auto &value: values) {
				initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, value}});
				CHECK(true == Config::get()->getBool(Config::CONFIG_KEY_PORT));
				STD_FILESYSTEM::remove(iniFilename);
			}
		}

		SUBCASE("All possible values for false") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::vector<std::string> values = {
				"0", "f", "F", "false", "False", "FALSE", "fAlse", "faLse",
				"falSe", "falsE", "FAlse", "FaLse", "FalSe", "FalsE",
				"fALse", "fAlSe", "fAlsE", "faLSe", "falSE", "FALse",
				"FAlSe", "FAlsE", "fALSe", "fALsE", "faLSE"
			};

			for (const auto &value: values) {
				initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, value}});
				CHECK(false == Config::get()->getBool(Config::CONFIG_KEY_PORT));
				STD_FILESYSTEM::remove(iniFilename);
			}
		}

		SUBCASE("Invalid value") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";

			// Yes, no, y, and n should be considered valid boolean values,
			// and in a future release I'll probably consider them valid.
			std::vector<std::string> values = {
				"blah", "yes", "no", "y", "n", "5", "-5", "1.3", "-1.3",
				"inf", "", "$"
			};

			for (const auto &value: values) {

				initIniFile(iniFilename, {{Config::CONFIG_KEY_PORT, value}});

				try {
					Config::get()->getBool(Config::CONFIG_KEY_PORT);
					CHECK(false);
				} catch (const ConfigInvalidValue &e) {
					CHECK(true);
				}

				STD_FILESYSTEM::remove(iniFilename);
			}
		}
	}

	TEST_CASE ("Config (config.cpp): getStatePath()") {

		SUBCASE("Default value") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{}});

			std::string expected = std::string(TROGDORD_INSTALL_PREFIX) +
				STD_FILESYSTEM::path::preferred_separator +
				Config::DEFAULTS.find(Config::CONFIG_KEY_STATE_PATH)->second;

			CHECK(0 == expected.compare(Config::get()->getStatePath()));
		}

		SUBCASE("Relative path") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_STATE_PATH, "test"}});

			std::string expected = std::string(TROGDORD_INSTALL_PREFIX) +
				STD_FILESYSTEM::path::preferred_separator + "test";

			CHECK(0 == expected.compare(Config::get()->getStatePath()));
		}

		SUBCASE("Absolute path") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_STATE_PATH, "/test"}});

			CHECK(0 == std::string("/test").compare(Config::get()->getStatePath()));
		}
	}

	TEST_CASE ("Config (config.cpp): initErrorLogger() and err()") {

		SUBCASE("cout") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_LOGTO, "stdout"}});

			CHECK(Config::get()->err().streamEquals(&std::cout));
		}

		SUBCASE("cerr") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_LOGTO, "stderr"}});

			CHECK(Config::get()->err().streamEquals(&std::cerr));
		}

		SUBCASE("file") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string logFile = STD_FILESYSTEM::temp_directory_path().string() + "/test.log";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_LOGTO, logFile}});

			CHECK(STD_FILESYSTEM::exists(logFile));
		}
	}
}
