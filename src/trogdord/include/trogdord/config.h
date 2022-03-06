#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

// What we return to the shell when trogdord aborts due to an error reading the
// ini file
#define EXIT_INI_ERROR 2

#include <string>
#include <memory>
#include <unordered_map>
#include <typeinfo>

#include <ini.h>
#include <trogdor/utility.h>

#include "io/iostream/servererr.h"
#include "exception/configinvalidvalue.h"
#include "exception/configundefinedvalue.h"

// All relative paths in trogdord.ini are computed based on this root directory.
// This should be provided by the build system, but if it's not, we'll assume /.
#ifndef TROGDORD_INSTALL_PREFIX
	#define TROGDORD_INSTALL_PREFIX "/"
#endif

class Config {

	protected:

		// Singleton instance of Config.
		static std::unique_ptr<Config> instance;

		// The file where we read our ini values from
		std::string iniPath;

		// Parsed ini file
		std::unordered_map<std::string, std::string> ini;

		// Our global error logger
		std::unique_ptr<ServerErr> errStream;

		// If not nullptr, this is the file we log errors to (otherwise, we're
		// logging to stdout or stderr.)
		std::unique_ptr<std::ofstream> logFileStream;

		// Protected constructor, making get() the only way to return an
		// instance.
		explicit Config(std::string newIniPath) noexcept;
		Config() = delete;
		Config(const Config &) = delete;

		// Initialize the global error logger.
		void initErrorLogger() noexcept;

		// Converts a relative path to an absolute path relative to trogdord's
		// installation directory and returns absolute paths unchanged.
		std::string relativeToAbsolutePath(std::string relative);

	public:

		// Maps env variables to config options they override
		static const std::unordered_map<std::string, std::string> ENV;

		// Default values
		static const std::unordered_map<std::string, std::string> DEFAULTS;

		// Determines which values should be hidden from the outside
		// for security purposes
		static const std::unordered_map<std::string, bool> hidden;

		// Determines what type each setting should be when its value is retrieved
		static const std::unordered_map<std::string, const std::type_info *> types;

		// Ini file keys
		static constexpr const char *CONFIG_KEY_PORT = "network.port";
		static constexpr const char *CONFIG_KEY_REUSE_ADDRESS = "network.reuse_address";
		static constexpr const char *CONFIG_KEY_SEND_TCP_KEEPALIVE = "network.send_keepalive";
		static constexpr const char *CONFIG_KEY_LISTEN_IPS = "network.listen";
		static constexpr const char *CONFIG_KEY_LOGTO = "logging.logto";
		static constexpr const char *CONFIG_KEY_INPUT_LISTENERS = "input.listeners";
		static constexpr const char *CONFIG_KEY_OUTPUT_DRIVER = "output.driver";
		static constexpr const char *CONFIG_KEY_REDIS_HOST = "redis.host";
		static constexpr const char *CONFIG_KEY_REDIS_USERNAME = "redis.username";
		static constexpr const char *CONFIG_KEY_REDIS_PASSWORD = "redis.password";
		static constexpr const char *CONFIG_KEY_REDIS_PORT = "redis.port";
		static constexpr const char *CONFIG_KEY_REDIS_CONNECTION_TIMEOUT = "redis.connection_timeout";
		static constexpr const char *CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL = "redis.connection_retry_interval";
		static constexpr const char *CONFIG_KEY_REDIS_OUTPUT_CHANNEL = "redis.output_channel";
		static constexpr const char *CONFIG_KEY_REDIS_INPUT_CHANNEL = "redis.input_channel";
		static constexpr const char *CONFIG_KEY_DEFINITIONS_PATH = "resources.definitions_path";
		static constexpr const char *CONFIG_KEY_STATE_ENABLED = "state.enabled";
		static constexpr const char *CONFIG_KEY_STATE_AUTORESTORE_ENABLED = "state.auto_restore";
		static constexpr const char *CONFIG_KEY_STATE_DUMP_SHUTDOWN_ENABLED = "state.dump_on_shutdown";
		static constexpr const char *CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED = "state.crash_recovery";
		static constexpr const char *CONFIG_KEY_STATE_FORMAT = "state.format";
		static constexpr const char *CONFIG_KEY_STATE_PATH = "state.save_path";
		static constexpr const char *CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME = "state.max_dumps_per_game";
		static constexpr const char *CONFIG_KEY_EXTENSIONS_PATH = "extensions.path";
		static constexpr const char *CONFIG_KEY_EXTENSIONS_LOAD = "extensions.load";

		// Returns singleton instance of Config.
		static std::unique_ptr<Config> &get() noexcept;

		// Loads the specified ini file if it exists. If it doesn't, or if
		// no file path is passed into the first argument, all config values
		// will be set to their defaults. Calling this method with no argument
		// can be used to reset the Config object.
		void load(std::string newIniPath = "") noexcept;

		// Provide const iterators to walk through each config setting.
		const auto begin() const {return ini.cbegin();}
		const auto end() const {return ini.cend();}

		// Resets an ini file setting to its hardcoded default value.
		void setDefaultValue(std::string key) {

			if (ini.end() != ini.find(key) && DEFAULTS.end() != DEFAULTS.find(key)) {
				ini[key] = DEFAULTS.find(key)->second;
			}
		}

		// Returns a vector of all extensions that are to be loaded when the
		// server starts.
		std::vector<std::string> getExtensions();

		// Special helper function that returns the absolute path pointed to
		// by CONFIG_KEY_EXTENSIONS_PATH or a blank string if no path was set.
		std::string getExtensionsPath() {

			return relativeToAbsolutePath(getString(CONFIG_KEY_EXTENSIONS_PATH));
		}

		// Special helper function that returns the absolute path pointed to
		// by CONFIG_KEY_STATE_PATH or a blank string if no path was set.
		std::string getStatePath() {

			return relativeToAbsolutePath(getString(CONFIG_KEY_STATE_PATH));
		}

		// Returns the specified config value. Throws ConfigUndefinedValue
		// if the config value isn't set and ConfigInvalidValue if it
		// cannot be cast to the requested type.
		std::string getString(std::string key) {

			if (ini.end() == ini.find(key)) {
				throw ConfigUndefinedValue("Config value '" + key + "' is undefined.");
			}

			return ini[key];
		}

		// Returns the specified config value as a boolean. Throws
		// ConfigUndefinedValue if the config value isn't set and
		// ConfigInvalidValue if it cannot be cast to the requested type.
		bool getBool(std::string key) {

			if (ini.end() == ini.find(key)) {
				throw ConfigUndefinedValue("Config value '" + key + "' is undefined.");
			}

			std::string value = trogdor::strToLower(trogdor::trim(ini[key]));

			if (0 == value.compare("1") || 0 == value.compare("true") || 0 == value.compare("t")) {
				return true;
			}

			else if (0 == value.compare("0") || 0 == value.compare("false") || 0 == value.compare("f")) {
				return false;
			}

			else {
				throw ConfigInvalidValue("Config value '" + key + "' is invalid.");
			}
		}

		// Returns the specified config value as an int. Throws
		// ConfigUndefinedValue if the config value isn't set and
		// ConfigInvalidValue if it cannot be cast to the requested type.
		int getInt(std::string key) {

			if (ini.end() == ini.find(key)) {
				throw ConfigUndefinedValue("Config value '" + key + "' is undefined.");
			}

			// std::stoi() supports "true" and "false", but for what I'm
			// doing, that doesn't make sense, so I want to consider those
			// invalid.
			else if (0 == ini[key].compare("true") || 0 == ini[key].compare("false")) {
				throw ConfigInvalidValue("Config value '" + key + "' is invalid.");
			}

			// std::stoi() accepts decimal values with truncation, but I want
			// to consider these values invalid.
			else if (std::string::npos != ini[key].find(".")) {
				throw ConfigInvalidValue("Config value '" + key + "' is invalid.");
			}

			try {
				return std::stoi(ini[key]);
			}

			catch (const std::exception &e) {
				throw ConfigInvalidValue("Config value '" + key + "' is invalid.");
			}
		}

		// Returns the specified config value as a size_t. Throws
		// ConfigUndefinedValue if the config value isn't set and
		// ConfigInvalidValue if it cannot be cast to the requested type.
		size_t getUInt(std::string key) {

			if (ini.end() == ini.find(key)) {
				throw ConfigUndefinedValue("Config value '" + key + "' is undefined.");
			}

			// Consider it an invalid unsigned int if it has a decimal or a
			// negative sign (without this check, I either get truncated
			// integers or two's-compliment negatives expressed as large
			// positive integers.)
			else if (std::string::npos != ini[key].find(".") || std::string::npos != ini[key].find("-")) {
				throw ConfigInvalidValue("Config value '" + key + "' is invalid.");
			}

			size_t retVal;
			std::stringstream sstream(ini[key]);

			sstream >> retVal;

			if (sstream.fail()) {
				throw ConfigInvalidValue("Config value '" + key + "' is invalid.");
			}

			return retVal;
		}

		// Used to log errors to the global error handler. Optional argument
		// sets the severity level (see include/trogdor/iostream/trogerr.h for
		// details.)
		//
		// This example logs at severity level ERROR:
		//
		// Config::get()->err() << "This is an error!" << std::endl;
		//
		// And this example logs to severity level INFO:
		//
		// Config::Get()->err(trogdor::Trogerr::INFO) << "This is an info message!" << std::endl;
		inline ServerErr &err(trogdor::Trogerr::ErrorLevel severity = trogdor::Trogerr::ERROR) {

			errStream->setErrorLevel(severity);
			return *errStream;
		}

		// Returns true if the error log is writing to a file and false if
		// it's writing to stderr or stdout.
		inline bool isErrorLogFile() {return logFileStream ? true : false;}
};


#endif
