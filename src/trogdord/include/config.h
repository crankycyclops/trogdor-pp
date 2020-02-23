#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

// What we return to the shell when trogdord aborts due to an error reading the
// ini file
#define EXIT_INI_ERROR 2

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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

		// Default values
		static const std::unordered_map<std::string, std::string> DEFAULTS;

		// Singleton instance of Config.
		static std::unique_ptr<Config> instance;

		// Parsed ini file
		boost::property_tree::iptree ini;

		// Our global error logger
		std::unique_ptr<ServerErr> errStream;

		// If not nullptr, this is the file we log errors to (otherwise, we're
		// logging to stdout or stderr.)
		std::unique_ptr<std::ofstream> logFileStream;

		// Protected constructor, making get() the only way to return an
		// instance.
		Config(std::string iniPath) noexcept;
		Config() = delete;
		Config(const Config &) = delete;

		// Initialize the global error logger.
		void initErrorLogger() noexcept;

	public:

		// Setting keys
		static const char *CONFIG_KEY_PORT;
		static const char *CONFIG_KEY_REUSE_ADDRESS;
		static const char *CONFIG_KEY_SEND_TCP_KEEPALIVE;
		static const char *CONFIG_KEY_LOGTO;
		static const char *CONFIG_KEY_DEFINITIONS_PATH;
		static const char *CONFIG_KEY_OUTPUT_DRIVER;

		// Returns singleton instance of Config.
		static std::unique_ptr<Config> &get() noexcept;

		// Returns the specified config value. Throws ConfigUndefinedValue
		// if the config value isn't set and ConfigInvalidValue if it
		// cannot be cast to the requested type.
		template<typename returnType> returnType value(std::string key) {

			try {
				return ini.get<returnType>(key);
			}

			catch (boost::property_tree::ptree_bad_path &e) {
				throw ConfigUndefinedValue("Config value '" + key + "' is undefined.");
			}

			catch (boost::property_tree::ptree_bad_data &e) {
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
