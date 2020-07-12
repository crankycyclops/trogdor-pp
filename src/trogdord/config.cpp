#include <cstdlib>
#include <iostream>

#include <trogdor/utility.h>
#include <trogdor/filesystem.h>

#include "include/config.h"

#ifndef TROGDORD_INI_PATH
	#define TROGDORD_INI_PATH ""
#endif


// Ini file keys
const char *Config::CONFIG_KEY_PORT = "network.port";
const char *Config::CONFIG_KEY_DEFINITIONS_PATH = "resources.definitions_path";
const char *Config::CONFIG_KEY_REUSE_ADDRESS = "network.reuse_address";
const char *Config::CONFIG_KEY_SEND_TCP_KEEPALIVE = "network.send_keepalive";
const char *Config::CONFIG_KEY_LOGTO = "logging.logto";
const char *Config::CONFIG_KEY_INPUT_LISTENERS = "input.listeners";
const char *Config::CONFIG_KEY_OUTPUT_DRIVER = "output.driver";
const char *Config::CONFIG_KEY_REDIS_HOST = "redis.host";
const char *Config::CONFIG_KEY_REDIS_PORT = "redis.port";
const char *Config::CONFIG_KEY_REDIS_CONNECTION_TIMEOUT = "redis.connection_timeout";
const char *Config::CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL = "redis.connection_retry_interval";
const char *Config::CONFIG_KEY_REDIS_OUTPUT_CHANNEL = "redis.output_channel";
const char *Config::CONFIG_KEY_REDIS_INPUT_CHANNEL = "redis.input_channel";

// Default ini values
const std::unordered_map<std::string, std::string> Config::DEFAULTS = {
	{CONFIG_KEY_PORT,                             "1040"},
	{CONFIG_KEY_REUSE_ADDRESS,                    "true"},
	{CONFIG_KEY_SEND_TCP_KEEPALIVE,               "true"},
	{CONFIG_KEY_LOGTO,                            "stderr"},
	{CONFIG_KEY_DEFINITIONS_PATH,                 "share/trogdor"},
	{CONFIG_KEY_INPUT_LISTENERS,                  "[]"},
	{CONFIG_KEY_OUTPUT_DRIVER,                    "local"},
	{CONFIG_KEY_REDIS_HOST,                       "localhost"},
	{CONFIG_KEY_REDIS_PORT,                       "6379"},
	{CONFIG_KEY_REDIS_CONNECTION_TIMEOUT,         "5000"},
	{CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL,  "5000"},
	{CONFIG_KEY_REDIS_OUTPUT_CHANNEL,             "trogdord:out"},
	{CONFIG_KEY_REDIS_INPUT_CHANNEL,              "trogdord:in"}
};

// Singleton instance of Config
std::unique_ptr<Config> Config::instance = nullptr;

/*****************************************************************************/

Config::Config(std::string newIniPath) noexcept {

	// Load the ini file's values
	load(newIniPath);

	// Setup the global error logger
	initErrorLogger();
}

/*****************************************************************************/

void Config::initErrorLogger() noexcept {

	logFileStream = nullptr;

	std::string logto = trogdor::strToLower(ini.get<std::string>(CONFIG_KEY_LOGTO));

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

std::unique_ptr<Config> &Config::get() noexcept {

	if (!instance) {
		instance = std::unique_ptr<Config>(new Config(TROGDORD_INI_PATH));
	}

	return instance;
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
		try {
			boost::property_tree::ini_parser::read_ini(iniPath, ini);
		}

		// Error logging hasn't been setup yet, so I have to write to stderr.
		catch (const boost::property_tree::ini_parser::ini_parser_error &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			exit(EXIT_INI_ERROR);
		}
	}

	// Populate the ini object with defaults for any values not set in the ini
	// file.
	for (auto &defaultVal: DEFAULTS) {

		boost::optional iniValue = ini.get_optional<std::string>(defaultVal.first);

		if (!iniValue) {
			ini.put(defaultVal.first, defaultVal.second);
		}
	}
}
