#include <string>
#include <sstream>

#include "include/json.h"


std::string JSON::serialize(JSONObject obj) {

	std::stringstream jsonStream;

	write_json(jsonStream, obj, false);
	return jsonStream.str();
}

/*****************************************************************************/

JSONObject JSON::deserialize(std::string json) {

	JSONObject obj;
	std::stringstream jsonStream(json);

	read_json(jsonStream, obj);
	return obj;
}
