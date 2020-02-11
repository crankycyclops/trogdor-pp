#include <cstdlib>
#include <iostream>

#include "include/config.h"

#ifdef __cpp_lib_filesystem
	#include <filesystem>
	#define STD_FILESYSTEM std::filesystem
#else
	#include <experimental/filesystem>
	#define STD_FILESYSTEM std::experimental::filesystem
#endif

#ifndef TROGDORD_INI_PATH
	#define TROGDORD_INI_PATH ""
#endif


// Ini file keys
const char *Config::CONFIG_KEY_PORT = "network.port";
const char *Config::CONFIG_KEY_DEFINITIONS_PATH = "resources.definitions_path";

// Default ini values
const std::unordered_map<std::string, std::string> Config::DEFAULTS = {
	{CONFIG_KEY_PORT,             "1040"},
	{CONFIG_KEY_DEFINITIONS_PATH, "share/trogdor"}
};

// Default values
const char *DEFAULT_VALUE_PORT;
const char *DEFAULT_VALUE_DEFINITIONS_PATH;

// Singleton instance of Config
std::unique_ptr<Config> Config::instance = nullptr;

/*****************************************************************************/

Config::Config(std::string iniPath) {

	if (
		0 != iniPath.compare("") &&
		STD_FILESYSTEM::exists(iniPath) &&
		!STD_FILESYSTEM::is_directory(iniPath)
	) {
		try {
			boost::property_tree::ini_parser::read_ini(iniPath, ini);
		}

		// TODO: log to error stream instead and remove include of iostream
		catch (boost::property_tree::ini_parser::ini_parser_error &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			exit(EXIT_INI_ERROR);
		}
	}

	// Populate the ini object with defaults for any values not set in the
	// ini file.
	for (auto &defaultVal: DEFAULTS) {
		if (ini.not_found() == ini.find(defaultVal.first)) {
			ini.put(defaultVal.first, defaultVal.second);
		}
	}
}

/*****************************************************************************/

std::unique_ptr<Config> &Config::get() {

	if (!instance) {
		instance = std::unique_ptr<Config>(new Config(TROGDORD_INI_PATH));
	}

	return instance;
}
