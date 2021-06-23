#ifndef PHP_JSON_H
#define PHP_JSON_H

#include <string>
#include <rapidjson/document.h>

extern "C" {
	#include "php.h"
}

using namespace rapidjson;


// An interface for serializing and deserializing JSON objects
class JSON {

	private:

		// The maximum depth of a zval array (prevents crashing due to
		// excessive recursion.)
		static const int ZVAL_TO_JSON_MAX_DEPTH = 150;

		// Called by JSONToZval to convert an array
		static zval ArrayToZval(Value const &value);

		// Called by JSONToZval to convert an object
		static zval ObjectToZval(Value const &value);

		// Called by JSONToZval to convert a scalar value
		static zval ScalarToZval(Value const &value);

		// Called by ZvalToJSON to convert a zval numerically indexed array to
		// a JSON value. The depth value should only be passed when called by
		// itself recursively, as its only intended use is to prevent us from
		// recursing so deeply that we crash.
		static Value ArrayZvalToJSON(Document &result, zval *z, int depth);

		// Called by ZvalToJSON to convert a zval associative array to a JSON
		// value
		static Value ObjectZvalToJSON(Document &result, zval *z, int depth);

		// Called by ZvalToJSON to convert a zval scalar to a JSON value
		static Value ScalarZvalToJSON(Document &result, zval *z, int depth);

		// Called by ZvalToJSON to convert a zval to a JSON value. The public
		// facing method will return a Document, but this needs to be able to
		// return a Value so it can be called recursively.
		static Value ZvalToJSONValue(Document &result, zval *z, int depth);

		// Returns HASH_KEY_IS_STRING if the first value of the PHP array has
		// an associative index and HASH_KEY_IS_LONG if the value has a
		// numeric index.
		static int GetZvalArrayType(zval *z);

	public:

		// Escapes a string so that it can be inserted into raw JSON
		static std::string escape(const std::string &value);

		// Document -> JSON string
		static std::string serialize(Document document);

		// JSON string -> Document
		static Document deserialize(std::string json);

		// Document -> Zval (note that Document inherits from Value, so you
		// can pass in either one.) This handles arrays, objects, and scalar
		// values.
		static zval JSONToZval(Value const &value);

		// Zval -> Document. Like JSONToZval, this handles arrays (all
		// numerically indexed or all associative) and scalars.
		static Document ZvalToJSON(zval *z);
};


#endif
