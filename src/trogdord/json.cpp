#include <string>
#include <sstream>
#include <regex>

#include <trogdor/utility.h>
#include <boost/algorithm/string.hpp>

#include "include/json.h"


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

	// Hack #3 to support boolean types (*sigh* again)
	trogdor::strReplace(json, "\"\\\\true\\\\\"", "true");
	trogdor::strReplace(json, "\"\\\\false\\\\\"", "false");

	return json;
}

/*****************************************************************************/

JSONObject JSON::deserialize(std::string json) {

	JSONObject obj;
	std::stringstream jsonStream(json);

	read_json(jsonStream, obj);
	return obj;
}
