#include <cstdlib>
#include <fstream>
#include <iostream>

#include <trogdor/utility.h>
#include <trogdor/filesystem.h>

#include "include/config.h"

#ifndef TROGDORD_INI_PATH
	#define TROGDORD_INI_PATH ""
#endif


// Default ini values
const std::unordered_map<std::string, std::string> Config::DEFAULTS = {

	{CONFIG_KEY_PORT,                             "1040"},
	{CONFIG_KEY_REUSE_ADDRESS,                    "true"},
	{CONFIG_KEY_SEND_TCP_KEEPALIVE,               "true"},
	{CONFIG_KEY_LOGTO,                            "stderr"},
	{CONFIG_KEY_INPUT_LISTENERS,                  "[]"},
	{CONFIG_KEY_OUTPUT_DRIVER,                    "local"},
	{CONFIG_KEY_REDIS_HOST,                       "localhost"},
	{CONFIG_KEY_REDIS_PORT,                       "6379"},
	{CONFIG_KEY_REDIS_CONNECTION_TIMEOUT,         "5000"},
	{CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL,  "5000"},
	{CONFIG_KEY_REDIS_OUTPUT_CHANNEL,             "trogdord:out"},
	{CONFIG_KEY_REDIS_INPUT_CHANNEL,              "trogdord:in"},
	{CONFIG_KEY_DEFINITIONS_PATH,                 "share/trogdor"},

	#ifdef ENABLE_SERIALIZE_JSON
		{CONFIG_KEY_STATE_ENABLED,               "true"},
	#else
		{CONFIG_KEY_STATE_ENABLED,               "false"},
	#endif

	{CONFIG_KEY_STATE_FORMAT,                     "json"},
	{CONFIG_KEY_STATE_PATH,                       "var/trogdord/state"}
};

// Singleton instance of Config
std::unique_ptr<Config> Config::instance = nullptr;


/*****************************************************************************/

Config::Config(std::string newIniPath) noexcept {

	// Load the ini file's values
	load(newIniPath);

	// Setup global error logger
	initErrorLogger();
}

/*****************************************************************************/

std::unique_ptr<Config> &Config::get() noexcept {

	if (!instance) {
		instance = std::unique_ptr<Config>(new Config(TROGDORD_INI_PATH));
	}

	return instance;
}

/*****************************************************************************/

void Config::initErrorLogger() noexcept {

	logFileStream = nullptr;

	std::string logto = trogdor::strToLower(getString(CONFIG_KEY_LOGTO));

	if (0 == logto.compare("stderr")) {
		errStream = std::make_unique<ServerErr>(&std::cerr);
	}

	else if (0 == logto.compare("stdout")) {
		errStream = std::make_unique<ServerErr>(&std::cout);
	}

	else {

		// If the configured path is relative, compute an absolute path based
		// on the installation prefix.
		if (STD_FILESYSTEM::path(logto).is_relative()) {

			std::string prefix = TROGDORD_INSTALL_PREFIX;

			if (prefix[prefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
				prefix += STD_FILESYSTEM::path::preferred_separator;
			}

			logto = prefix + logto;
		}

		try {
			logFileStream = std::make_unique<std::ofstream>(logto, std::ofstream::app);
			errStream = std::make_unique<ServerErr>(logFileStream.get());
		}

		// If for whatever reason we failed to open the file for writing
		// (maybe the process doesn't have the necessary permissions?), fall
		// back to std::cerr.
		catch (const std::exception &e) {

			std::cerr << e.what() << std::endl;
			std::cerr << "WARNING: failed to open " << logto << " for writing. Falling back to stderr." << std::endl;

			logFileStream = nullptr;
			errStream = std::make_unique<ServerErr>(&std::cerr);
		}
	}
}

/*****************************************************************************/

void Config::load(std::string newIniPath) noexcept {

	iniPath = newIniPath;
	ini.clear();

	if (
		0 != iniPath.compare("") &&
		STD_FILESYSTEM::exists(iniPath) &&
		!STD_FILESYSTEM::is_directory(iniPath)
	) {
		int status = ini_parse(iniPath.c_str(), [](
			void *data,
			const char *section,
			const char *name,
			const char *value
		) -> int {

			std::string key = section ? std::string(section) + '.' + name : name;

			static_cast<Config *>(data)->ini[key] = value;
			return 1;

		}, this);

		if (-1 == status) {
			std::cerr << "Error: " << "Couldn't open " << iniPath
				<< " for reading." << std::endl;
			exit(EXIT_INI_ERROR);
		}

		else if (-2 == status) {
			std::cerr << "Error: " << "A memory allocation error occured when attempting to parse "
				<< iniPath << '.' << std::endl;
			exit(EXIT_INI_ERROR);
		}

		else if (0 != status) {
			std::cerr << "Error: " << "An unknown error occured when attempting to parse "
				<< iniPath << '.' << std::endl;
		}
	}

	// Populate the ini object with defaults for any values not set in the ini
	// file.
	for (auto &defaultVal: DEFAULTS) {
		if (ini.end() == ini.find(defaultVal.first)) {
			ini[defaultVal.first] = defaultVal.second;
		}
	}
}
