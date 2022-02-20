#include <cstdlib>
#include <fstream>
#include <iostream>

#include <trogdord/json.h>
#include <trogdor/utility.h>
#include <trogdor/filesystem.h>

#include <trogdord/config.h>
#include <trogdord/filesystem.h>

#ifndef TROGDORD_INI_PATH
	#define TROGDORD_INI_PATH ""
#endif


// Maps env variables to config options they override
const std::unordered_map<std::string, std::string> Config::ENV = {

	{"TROGDORD_PORT",                             CONFIG_KEY_PORT},
	{"TROGDORD_REUSE_ADDRESS",                    CONFIG_KEY_REUSE_ADDRESS},
	{"TROGDORD_SEND_TCP_KEEPALIVE",               CONFIG_KEY_SEND_TCP_KEEPALIVE},
	{"TROGDORD_LISTEN_IPS",                       CONFIG_KEY_LISTEN_IPS},
	{"TROGDORD_LOGTO",                            CONFIG_KEY_LOGTO},
	{"TROGDORD_INPUT_LISTENERS",                  CONFIG_KEY_INPUT_LISTENERS},
	{"TROGDORD_OUTPUT_DRIVER",                    CONFIG_KEY_OUTPUT_DRIVER},
	{"TROGDORD_REDIS_HOST",                       CONFIG_KEY_REDIS_HOST},
	{"TROGDORD_REDIS_USERNAME",                   CONFIG_KEY_REDIS_USERNAME},
	{"TROGDORD_REDIS_PASSWORD",                   CONFIG_KEY_REDIS_PASSWORD},
	{"TROGDORD_REDIS_PORT",                       CONFIG_KEY_REDIS_PORT},
	{"TROGDORD_REDIS_CONNECTION_TIMEOUT",         CONFIG_KEY_REDIS_CONNECTION_TIMEOUT},
	{"TROGDORD_REDIS_CONNECTION_RETRY_INTERVAL",  CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL},
	{"TROGDORD_REDIS_OUTPUT_CHANNEL",             CONFIG_KEY_REDIS_OUTPUT_CHANNEL},
	{"TROGDORD_REDIS_INPUT_CHANNEL",              CONFIG_KEY_REDIS_INPUT_CHANNEL},
	{"TROGDORD_DEFINITIONS_PATH",                 CONFIG_KEY_DEFINITIONS_PATH},
	{"TROGDORD_STATE_ENABLED",                    CONFIG_KEY_STATE_ENABLED},
	{"TROGDORD_STATE_AUTORESTORE_ENABLED",        CONFIG_KEY_STATE_AUTORESTORE_ENABLED},
	{"TROGDORD_STATE_DUMP_SHUTDOWN_ENABLED",      CONFIG_KEY_STATE_DUMP_SHUTDOWN_ENABLED},
	{"TROGDORD_STATE_CRASH_RECOVERY_ENABLED",     CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED},
	{"TROGDORD_STATE_FORMAT",                     CONFIG_KEY_STATE_FORMAT},
	{"TROGDORD_STATE_PATH",                       CONFIG_KEY_STATE_PATH},
	{"TROGDORD_STATE_MAX_DUMPS_PER_GAME",         CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME},
	{"TROGDORD_EXTENSIONS_PATH",                  CONFIG_KEY_EXTENSIONS_PATH},
	{"TROGDORD_EXTENSIONS_LOAD",                  CONFIG_KEY_EXTENSIONS_LOAD}
};

// Default ini values
const std::unordered_map<std::string, std::string> Config::DEFAULTS = {

	{CONFIG_KEY_PORT,                             "1040"},
	{CONFIG_KEY_REUSE_ADDRESS,                    "true"},
	{CONFIG_KEY_SEND_TCP_KEEPALIVE,               "true"},
	{CONFIG_KEY_LISTEN_IPS,                       "[\"127.0.0.1\", \"::1\"]"},
	{CONFIG_KEY_LOGTO,                            "stderr"},
	{CONFIG_KEY_INPUT_LISTENERS,                  "[]"},
	{CONFIG_KEY_OUTPUT_DRIVER,                    "local"},
	{CONFIG_KEY_REDIS_HOST,                       "localhost"},
	{CONFIG_KEY_REDIS_USERNAME,                   ""},
	{CONFIG_KEY_REDIS_PASSWORD,                   ""},
	{CONFIG_KEY_REDIS_PORT,                       "6379"},
	{CONFIG_KEY_REDIS_CONNECTION_TIMEOUT,         "5000"},
	{CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL,  "5000"},
	{CONFIG_KEY_REDIS_OUTPUT_CHANNEL,             "trogdord:out"},
	{CONFIG_KEY_REDIS_INPUT_CHANNEL,              "trogdord:in"},
	{CONFIG_KEY_DEFINITIONS_PATH,                 "share/trogdor"},
	{CONFIG_KEY_STATE_ENABLED,                    "false"},
	{CONFIG_KEY_STATE_AUTORESTORE_ENABLED,        "false"},
	{CONFIG_KEY_STATE_DUMP_SHUTDOWN_ENABLED,      "false"},
	{CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED,     "false"},
	{CONFIG_KEY_STATE_FORMAT,                     "json"},
	{CONFIG_KEY_STATE_PATH,                       "var/trogdord/state"},
	{CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME,         "0"},
	{CONFIG_KEY_EXTENSIONS_PATH,                  "lib/trogdord"},
	{CONFIG_KEY_EXTENSIONS_LOAD,                  "[]"}
};

// Any setting that has a value of true will be considered sensitive
// information and will be hidden from the outside world.
const std::unordered_map<std::string, bool> Config::hidden = {

	{CONFIG_KEY_PORT,                             true},
	{CONFIG_KEY_REUSE_ADDRESS,                    true},
	{CONFIG_KEY_SEND_TCP_KEEPALIVE,               true},
	{CONFIG_KEY_LISTEN_IPS,                       true},
	{CONFIG_KEY_LOGTO,                            true},
	{CONFIG_KEY_INPUT_LISTENERS,                  true},
	{CONFIG_KEY_OUTPUT_DRIVER,                    false},
	{CONFIG_KEY_REDIS_HOST,                       true},
	{CONFIG_KEY_REDIS_USERNAME,                   true},
	{CONFIG_KEY_REDIS_PASSWORD,                   true},
	{CONFIG_KEY_REDIS_PORT,                       true},
	{CONFIG_KEY_REDIS_CONNECTION_TIMEOUT,         true},
	{CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL,  true},
	{CONFIG_KEY_REDIS_OUTPUT_CHANNEL,             false},
	{CONFIG_KEY_REDIS_INPUT_CHANNEL,              false},
	{CONFIG_KEY_DEFINITIONS_PATH,                 true},
	{CONFIG_KEY_STATE_ENABLED,                    false},
	{CONFIG_KEY_STATE_AUTORESTORE_ENABLED,        true},
	{CONFIG_KEY_STATE_DUMP_SHUTDOWN_ENABLED,      true},
	{CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED,     true},
	{CONFIG_KEY_STATE_FORMAT,                     true},
	{CONFIG_KEY_STATE_PATH,                       true},
	{CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME,         false},
	{CONFIG_KEY_EXTENSIONS_PATH,                  true},
	{CONFIG_KEY_EXTENSIONS_LOAD,                  false}
};

const std::unordered_map<std::string, const std::type_info *> Config::types = {

	{CONFIG_KEY_PORT,                             &typeid(int)},
	{CONFIG_KEY_REUSE_ADDRESS,                    &typeid(bool)},
	{CONFIG_KEY_SEND_TCP_KEEPALIVE,               &typeid(bool)},
	{CONFIG_KEY_LISTEN_IPS,                       &typeid(std::string)},
	{CONFIG_KEY_LOGTO,                            &typeid(std::string)},
	{CONFIG_KEY_INPUT_LISTENERS,                  &typeid(std::string)},
	{CONFIG_KEY_OUTPUT_DRIVER,                    &typeid(std::string)},
	{CONFIG_KEY_REDIS_HOST,                       &typeid(std::string)},
	{CONFIG_KEY_REDIS_USERNAME,                   &typeid(std::string)},
	{CONFIG_KEY_REDIS_PASSWORD,                   &typeid(std::string)},
	{CONFIG_KEY_REDIS_PORT,                       &typeid(int)},
	{CONFIG_KEY_REDIS_CONNECTION_TIMEOUT,         &typeid(int)},
	{CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL,  &typeid(int)},
	{CONFIG_KEY_REDIS_OUTPUT_CHANNEL,             &typeid(std::string)},
	{CONFIG_KEY_REDIS_INPUT_CHANNEL,              &typeid(std::string)},
	{CONFIG_KEY_DEFINITIONS_PATH,                 &typeid(std::string)},
	{CONFIG_KEY_STATE_ENABLED,                    &typeid(bool)},
	{CONFIG_KEY_STATE_AUTORESTORE_ENABLED,        &typeid(bool)},
	{CONFIG_KEY_STATE_DUMP_SHUTDOWN_ENABLED,      &typeid(bool)},
	{CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED,     &typeid(bool)},
	{CONFIG_KEY_STATE_FORMAT,                     &typeid(std::string)},
	{CONFIG_KEY_STATE_PATH,                       &typeid(std::string)},
	{CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME,         &typeid(int)},
	{CONFIG_KEY_EXTENSIONS_PATH,                  &typeid(std::string)},
	{CONFIG_KEY_EXTENSIONS_LOAD,                  &typeid(std::string)}
};

// Singleton instance of Config
std::unique_ptr<Config> Config::instance = nullptr;

/*****************************************************************************/

Config::Config(std::string newIniPath) noexcept {

	// Load the ini file's values
	load(newIniPath);
}

/*****************************************************************************/

std::unique_ptr<Config> &Config::get() noexcept {

	if (!instance) {
		instance = std::unique_ptr<Config>(new Config(TROGDORD_INI_PATH));
	}

	return instance;
}

/*****************************************************************************/

std::string Config::relativeToAbsolutePath(std::string relative) {

	relative = trogdor::trim(relative);

	if (0 != relative.compare("")) {
		relative = Filesystem::getAbsolutePath(relative);
	}

	return relative;
}

/*****************************************************************************/

std::vector<std::string> Config::getExtensions() {

	rapidjson::Document extensionsArr;

	std::string extensionsStr = Config::get()->getString(
		Config::CONFIG_KEY_EXTENSIONS_LOAD
	);

	extensionsArr.Parse(extensionsStr.c_str());

	if (extensionsArr.HasParseError()) {
		throw ServerException(
			std::string("invalid trogdord.ini value for ") +
			Config::CONFIG_KEY_EXTENSIONS_LOAD
		);
	}

	else if (rapidjson::kArrayType != extensionsArr.GetType()) {
		throw ServerException(
			std::string("trogdord.ini value for ") +
			Config::CONFIG_KEY_EXTENSIONS_LOAD +
			" must be a JSON array of strings."
		);
	}

	std::vector<std::string> extensions;

	for (auto i = extensionsArr.Begin(); i != extensionsArr.End(); i++) {
		extensions.push_back(i->GetString());
	}

	return extensions;
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

		logto = Filesystem::getAbsolutePath(logto);

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

	iniPath = trogdor::trim(newIniPath);
	ini.clear();

	// Start by populate the ini object with its default values
	for (auto &defaultVal: DEFAULTS) {
		ini[defaultVal.first] = defaultVal.second;
	}

	// Next, override the defaults with any options in the ini file
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

	// Finally, override any settings that have been configured in environment
	// variables (these take precedence over trogdord.ini.)
	for (auto &envVar: ENV) {
		if (const char *setting = std::getenv(envVar.first.c_str()); setting) {
			ini[envVar.second] = setting;
		}
	}

	// Setup global error logger
	initErrorLogger();
}
