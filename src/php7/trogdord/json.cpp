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

HashTable *JSON::JSONToHashTable(JSONObject obj) {

	// Used to match and cast values to types other than strings
	static std::regex integer("[+\\-]?[0-9]+");
	static std::regex decimal("[+\\-]?[0-9]*\\.[0-9]+");

	HashTable *ht;

	ALLOC_HASHTABLE(ht);

	// If I use NULL instead of ZVAL_PTR_DTOR, I end up with a memory leak
	zend_hash_init(ht, 0, NULL, ZVAL_PTR_DTOR, 0);

	for(auto pair: obj) {

		zval zData;

		if (pair.second.data().empty()) {
			ZVAL_ARR(&zData, JSONToHashTable(pair.second));
		}

		else {

			std::string value = pair.second.data();

			if (std::regex_match(value, integer)) {
				ZVAL_LONG(&zData, std::stoi(value));
			}

			else if (std::regex_match(value, decimal)) {
				ZVAL_DOUBLE(&zData, std::stod(value));
			}

			else {
				ZVAL_STRING(&zData, value.c_str());
			}
		}

		// Numerical index
		if (0 == pair.first.compare("")) {
			zend_hash_next_index_insert(ht, &zData);
			Z_ADDREF_P(&zData);
		}

		else {
			zend_hash_str_update(ht, pair.first.c_str(), pair.first.length(), &zData);
		}
	}

	return ht;
}

/*****************************************************************************/

zval JSON::JSONToZval(JSONObject obj) {

	zval array;
	HashTable *ht = JSONToHashTable(obj);

	ZVAL_ARR(&array, ht);
	return array;
}
