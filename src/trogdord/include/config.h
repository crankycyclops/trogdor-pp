#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

// What we return to the shell when trogdord aborts due to an error reading the
// ini file
#define EXIT_INI_ERROR 2

#include <string>
#include <memory>
#include <unordered_map>

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

	public:

		// Default values
		static const std::unordered_map<std::string, std::string> DEFAULTS;

		// Setting keys
		static const char *CONFIG_KEY_PORT;
		static const char *CONFIG_KEY_REUSE_ADDRESS;
		static const char *CONFIG_KEY_SEND_TCP_KEEPALIVE;
		static const char *CONFIG_KEY_LOGTO;
		static const char *CONFIG_KEY_DEFINITIONS_PATH;
		static const char *CONFIG_KEY_INPUT_LISTENERS;
		static const char *CONFIG_KEY_OUTPUT_DRIVER;
		static const char *CONFIG_KEY_REDIS_HOST;
		static const char *CONFIG_KEY_REDIS_PORT;
		static const char *CONFIG_KEY_REDIS_CONNECTION_TIMEOUT;
		static const char *CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL;
		static const char *CONFIG_KEY_REDIS_OUTPUT_CHANNEL;
		static const char *CONFIG_KEY_REDIS_INPUT_CHANNEL;

		// Returns singleton instance of Config.
		static std::unique_ptr<Config> &get() noexcept;

		// Loads the specified ini file if it exists. If it doesn't, or if
		// no file path is passed into the first argument, all config values
		// will be set to their defaults. Calling this method with no argument
		// can be used to reset the Config object.
		void load(std::string newIniPath = "") noexcept;

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

			std::stringstream sstream(ini[key]);
			size_t retVal;

			sstream >> retVal;
			return retVal;

			if (sstream.fail()) {
				throw ConfigInvalidValue("Config value '" + key + "' is invalid.");
			}
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
