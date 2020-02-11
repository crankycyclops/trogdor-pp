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

#include "exception/configinvalidvalue.h"
#include "exception/configundefinedvalue.h"


class Config {

	protected:

		// Default values
		static const std::unordered_map<std::string, std::string> DEFAULTS;

		// Singleton instance of Config.
		static std::unique_ptr<Config> instance;

		// Parsed ini file
		boost::property_tree::iptree ini;

		// Protected constructor, making get() the only way to return an
		// instance.
		Config(std::string iniPath);
		Config() = delete;
		Config(const Config &) = delete;

	public:

		// Setting keys
		static const char *CONFIG_KEY_PORT;
		static const char *CONFIG_KEY_DEFINITIONS_PATH;

		// Returns singleton instance of Config.
		static std::unique_ptr<Config> &get();

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
};


#endif
