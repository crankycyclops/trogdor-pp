#include <string>
#include <sstream>
#include <regex>

#include <boost/algorithm/string.hpp>

#include "json.h"


std::string JSON::serialize(JSONObject obj) {

	std::stringstream jsonStream;

	write_json(jsonStream, obj, false);
	std::string json = jsonStream.str();

	// Hack #1 to support empty arrays, since ptrees don't actually support arrays
	boost::replace_all(json, "[\"\"]", "[]");

	// Hack #2 to support numeric types, since ptrees don't do any typing either
	// (*sigh*)
	std::regex reg("\\\"([0-9]+\\.{0,1}[0-9]*)\\\"");
	json = std::regex_replace(json, reg, "$1");

	return json;
}

/*****************************************************************************/

JSONObject JSON::deserialize(std::string json) {

	JSONObject obj;
	std::stringstream jsonStream(json);

	read_json(jsonStream, obj);
	return obj;
}

/*****************************************************************************/

zval JSON::JSONToZval(JSONObject obj) {

	// Used to match and cast values to types other than strings
	static std::regex integer("[+\\-]?[0-9]+");
	static std::regex decimal("[+\\-]?[0-9]*\\.[0-9]+");

	zval array;
	array_init(&array);

	for(auto pair: obj) {

		zval zData;

		if (!pair.second.empty() && pair.second.data().empty()) {
			zData = JSONToZval(pair.second);
		}

		else {

			std::string value = pair.second.data();

			if (std::regex_match(value, integer)) {
				ZVAL_LONG(&zData, std::stoi(value));
			}

			else if (std::regex_match(value, decimal)) {
				ZVAL_DOUBLE(&zData, std::stod(value));
			}

			// Boolean detection is EXTREMELY error prone (it's very plausible
			// that I'll need a string with the values "true" or "false" and
			// get the wrong type as a result of this.) For now, there's not
			// much I can do, but in the long term, I'm going to have to
			// replace boost::ptree with a *real* JSON library.
			else if (value.compare("true")) {
				ZVAL_BOOL(&zData, 1);
			}

			else if (value.compare("false")) {
				ZVAL_BOOL(&zData, 0);
			}

			else {
				ZVAL_STRING(&zData, value.c_str());
			}
		}

		// Numerical index
		if (0 == pair.first.compare("")) {
			add_next_index_zval(&array, &zData);
		}

		else {
			add_assoc_zval(&array, pair.first.c_str(), &zData);
		}
	}

	return array;
}
