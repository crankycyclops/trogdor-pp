#include <doctest.h>
#include <fstream>

#include "../include/config.h"
#include "../include/filesystem.h"


TEST_SUITE("Config (config.cpp)") {

	TEST_CASE("Config (config.cpp): Construction without an ini file") {

		// I can't really construct this outside Config::get(), but I can
		// reset it by loading config values from an empty path.
		Config::get()->load("");

		// Make sure all config values are set to their defaults
		for (auto const &value: Config::get()->DEFAULTS) {
			CHECK(0 == value.second.compare(Config::get()->value<std::string>(value.first)));
		}
	}

	TEST_CASE("Config (config.cpp): Construction with an invalid ini file") {

		SUBCASE("ini file path points to file that doesn't exist") {

			Config::get()->load("/path/to/file/that/doesnt/exist");

			// Test to make sure that the config values are all set to their
			// defaults
			for (auto const &value: Config::get()->DEFAULTS) {
				CHECK(0 == value.second.compare(Config::get()->value<std::string>(value.first)));
			}
		}

		SUBCASE("ini file path is a directory") {

			// We know for a fact that this path exists and that it's a
			// directory.
			Config::get()->load(STD_FILESYSTEM::temp_directory_path());

			// Once again, test to make sure the config values are set to
			// their defaults
			for (auto const &value: Config::get()->DEFAULTS) {
				CHECK(0 == value.second.compare(Config::get()->value<std::string>(value.first)));
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
					CHECK(0 == Config::get()->value<std::string>(iniValue.first).compare("aDifferentValue"));
				} else {
					CHECK(0 == Config::get()->value<std::string>(iniValue.first).compare(iniValue.second));
				}
			}
		}

		STD_FILESYSTEM::remove(iniFilename);
	}
}
