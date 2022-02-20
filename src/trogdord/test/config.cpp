#include <doctest.h>
#include <unordered_set>

#include <rapidjson/document.h>

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

		// Make sure that, by defaullt, trogdord will only listen on 127.0.0.1 or ::1
		rapidjson::Document listenersArr;

		std::string listenersStr = Config::get()->getString(
			Config::CONFIG_KEY_LISTEN_IPS);

		listenersArr.Parse(listenersStr.c_str());

		if (listenersArr.HasParseError()) {
			FAIL("network.listen value is invalid");
		} else if (rapidjson::kArrayType != listenersArr.GetType()) {
			FAIL("network.listen value must be an array of strings");
		}

		for (auto listener = listenersArr.Begin(); listener != listenersArr.End(); listener++) {

			if (rapidjson::kStringType != listener->GetType()) {
				FAIL("network.listen value must be an array of strings");
			}

			else if (
				0 != std::string("127.0.0.1").compare(listener->GetString()) &&
				0 != std::string("::1").compare(listener->GetString())
			) {
				FAIL("network.listen should be set to listen only on localhost (127.0.0.1 or ::1) by default.");
			}
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

	TEST_CASE("Config (config.cpp): Construction with valid ini file and getString()") {

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

	TEST_CASE("Config (config.cpp): getUInt()") {

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

	TEST_CASE("Config (config.cpp): getInt()") {

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

	TEST_CASE("Config (config.cpp): getBool()") {

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

	TEST_CASE("Config (config.cpp): getStatePath()") {

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

	TEST_CASE("Config (config.cpp): getExtensionsPath()") {

		SUBCASE("Default value") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{}});

			std::string expected = std::string(TROGDORD_INSTALL_PREFIX) +
				STD_FILESYSTEM::path::preferred_separator +
				Config::DEFAULTS.find(Config::CONFIG_KEY_EXTENSIONS_PATH)->second;

			CHECK(0 == expected.compare(Config::get()->getExtensionsPath()));
		}

		SUBCASE("Relative path") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "test"}});

			std::string expected = std::string(TROGDORD_INSTALL_PREFIX) +
				STD_FILESYSTEM::path::preferred_separator + "test";

			CHECK(0 == expected.compare(Config::get()->getExtensionsPath()));
		}

		SUBCASE("Absolute path") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/test"}});

			CHECK(0 == std::string("/test").compare(Config::get()->getExtensionsPath()));
		}
	}

	TEST_CASE("Config (config.cpp): initErrorLogger() and err()") {

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
			STD_FILESYSTEM::remove(logFile);
		}
	}

	// In this test case, I want to verify that begin() and end() iterate
	// through every item exactly once
	TEST_CASE("Config (config.cpp): begin() and end()") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, Config::DEFAULTS);

			size_t nSettings = 0;
			std::unordered_set<std::string> seen;

			for (const auto &value: *Config::get()) {

				if (seen.end() != seen.find(value.first)) {
					FAIL("Iterator over Config::get() saw the same setting twice");
				}

				seen.insert(value.first);
				nSettings++;
			}

			CHECK(nSettings == Config::DEFAULTS.size());
			STD_FILESYSTEM::remove(iniFilename);
	}

	TEST_CASE("Config (config.cpp): setDefaultValue()") {

		SUBCASE("Ini value doesn't exist") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, Config::DEFAULTS);

			// Demonstate that we can't get the non-existant value
			try {
				Config::get()->getString("NotAValue");
				FAIL("Getting non-existant config value should throw ConfigUndefinedValue");
			} catch (const ConfigUndefinedValue &e) {}

			// Now, demonsrate that nothing happens when we attempt to set that
			// same key to its (undefined) default value
			Config::get()->setDefaultValue("NotAValue");

			// Prove that we still can't retrieve it, since it should
			// remain undefined
			try {
				Config::get()->getString("NotAValue");
				FAIL("Getting non-existant config value should throw ConfigUndefinedValue");
			} catch (const ConfigUndefinedValue &e) {}

			// Finally, show that no other config values were affected by the change
			for (const auto &defaultValue: Config::DEFAULTS) {
				if (0 != Config::get()->getString(defaultValue.first).compare(defaultValue.second)) {
					FAIL("Config::setDefaultValue() changed the values of one or more unintentional keys");
				}
			}

			STD_FILESYSTEM::remove(iniFilename);
		}

		SUBCASE("Initially not set") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";

			// Passing in an empty unordered_map results in setting no initial values
			initIniFile(iniFilename, {});

			// Demonstrate that getting the value without having set it returns
			// the default
			CHECK(0 == Config::get()->getString(Config::CONFIG_KEY_STATE_PATH).compare(Config::DEFAULTS.find(Config::CONFIG_KEY_STATE_PATH)->second));

			// Now, load the default value for that setting
			Config::get()->setDefaultValue(Config::CONFIG_KEY_STATE_PATH);

			// Finally, show that the value hasn't changed
			CHECK(0 == Config::get()->getString(Config::CONFIG_KEY_STATE_PATH).compare(Config::DEFAULTS.find(Config::CONFIG_KEY_STATE_PATH)->second));

			STD_FILESYSTEM::remove(iniFilename);
		}

		// This subcase should be functionally identical to the last one
		SUBCASE("Initially set to the default value") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, Config::DEFAULTS);

			// Demonstrate that getting the value without having set it returns
			// the default
			CHECK(0 == Config::get()->getString(Config::CONFIG_KEY_STATE_PATH).compare(Config::DEFAULTS.find(Config::CONFIG_KEY_STATE_PATH)->second));

			// Now, load the default value for that setting
			Config::get()->setDefaultValue(Config::CONFIG_KEY_STATE_PATH);

			// Finally, show that the value hasn't changed
			CHECK(0 == Config::get()->getString(Config::CONFIG_KEY_STATE_PATH).compare(Config::DEFAULTS.find(Config::CONFIG_KEY_STATE_PATH)->second));

			STD_FILESYSTEM::remove(iniFilename);
		}

		SUBCASE("Initially set to different value") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_STATE_PATH, "Flonkerton"}}); // Bonus points if you get the reference ;)

			// Verify that our setting was properly initialized
			CHECK(0 == Config::get()->getString(Config::CONFIG_KEY_STATE_PATH).compare("Flonkerton"));

			// Now, load the default value for that setting
			Config::get()->setDefaultValue(Config::CONFIG_KEY_STATE_PATH);

			// Finally, show that the setting has changed to its default value
			CHECK(0 == Config::get()->getString(Config::CONFIG_KEY_STATE_PATH).compare(Config::DEFAULTS.find(Config::CONFIG_KEY_STATE_PATH)->second));

			STD_FILESYSTEM::remove(iniFilename);
		}
	}

	TEST_CASE("Config (config.cpp): getExtensions()") {

		SUBCASE("Empty array") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_LOAD, "[]"}});

			CHECK(0 == Config::get()->getExtensions().size());
			STD_FILESYSTEM::remove(iniFilename);
		}

		SUBCASE("One Extension") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_LOAD, "[\"awesome.so\"]"}});

			CHECK(1 == Config::get()->getExtensions().size());
			CHECK(0 == Config::get()->getExtensions()[0].compare("awesome.so"));

			STD_FILESYSTEM::remove(iniFilename);
		}

		SUBCASE("Two Extensions") {

			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_LOAD, "[\"one.so\", \"two.so\"]"}});

			CHECK(2 == Config::get()->getExtensions().size());
			CHECK(0 == Config::get()->getExtensions()[0].compare("one.so"));
			CHECK(0 == Config::get()->getExtensions()[1].compare("two.so"));

			STD_FILESYSTEM::remove(iniFilename);
		}
	}
}
