#include <string>
#include <sstream>
#include <regex>

#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include <boost/algorithm/string.hpp>

#include "json.h"
#include "exception/jsonexception.h"

// TODO: remove
#include <iostream>

using namespace rapidjson;


// Private and called internally by JSONToZval
zval JSON::ArrayToZval(Value const &value) {

	zval array;
	array_init(&array);

	for (auto const &next: value.GetArray()) {
		zval zData = JSONToZval(next);
		add_next_index_zval(&array, &zData);
	}

	return array;
}

/*****************************************************************************/

// Private and called internally by JSONToZval
zval JSON::ObjectToZval(Value const &value) {

	zval object;
	array_init(&object);

	for (auto const &next: value.GetObject()) {
		zval zData = JSONToZval(next.value);
		add_assoc_zval(&object, next.name.GetString(), &zData);
	}

	return object;
}

/*****************************************************************************/

// Private and called internally by JSONToZval
zval JSON::ScalarToZval(Value const &value) {

	zval zData;

	switch(value.GetType()) {

		case kNullType:

			ZVAL_NULL(&zData);
			break;

		case kTrueType:

			ZVAL_BOOL(&zData, 1);
			break;

		case kFalseType:

			ZVAL_BOOL(&zData, 0);
			break;

		case kStringType:

			ZVAL_STRING(&zData, value.GetString());
			break;

		case kNumberType:

			if (value.IsDouble()) {
				ZVAL_DOUBLE(&zData, value.GetDouble());
			} else {
				#ifdef ZEND_ENABLE_ZVAL_LONG64
					ZVAL_LONG(&zData, value.GetInt64());
				#else
					ZVAL_LONG(&zData, value.GetInt());
				#endif
			}

			break;

		default:
			throw JSONException("Unknown JSON value type in ScalarToZval");
	}

	return zData;
}

/*****************************************************************************/

// Private and called internally by ZvalToJSON
int JSON::GetZvalArrayType(zval *z) {

	int type;
	HashPosition pos;

	zend_string *key;
	zend_ulong idx;

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(z), &pos);
	type = zend_hash_get_current_key_ex(Z_ARRVAL_P(z), &key, &idx, &pos);

	return type;
}

/*****************************************************************************/

// Private and called internally by ZvalToJSON
Value JSON::ArrayZvalToJSON(Document &result, zval *z, int depth) {

	// Prevent us from recursing so deeply that we crash the PHP process
	if (depth > ZVAL_TO_JSON_MAX_DEPTH) {
		throw JSONException(
			std::string("Maximum array depth of ") +
			std::to_string(ZVAL_TO_JSON_MAX_DEPTH) +
			" exceeded"
		);
	}

	// The resulting JSON Object
	Value root(kArrayType);

	// Current position in the PHP array
	HashPosition pos;

	for (
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(z), &pos);
		zend_hash_has_more_elements_ex(Z_ARRVAL_P(z), &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(z), &pos)
	) {
		Value JSONValue;

		int type;
		zval *data;

		zend_string *key;
		zend_ulong idx;

		type = zend_hash_get_current_key_ex(Z_ARRVAL_P(z), &key, &idx, &pos);
		data = zend_hash_get_current_data_ex(Z_ARRVAL_P(z), &pos);

		if (HASH_KEY_IS_LONG != type) {
			throw JSONException("Cannot convert an array with a mix of numeric and associative keys to JSON");
		}

		if (IS_ARRAY == Z_TYPE_P(data)) {

			if (HASH_KEY_IS_STRING == GetZvalArrayType(data)) {
				root.PushBack(ObjectZvalToJSON(result, data, depth + 1), result.GetAllocator());
			} else {
				root.PushBack(ArrayZvalToJSON(result, data, depth + 1), result.GetAllocator());
			}
		}

		else {
			root.PushBack(ScalarZvalToJSON(result, data, depth + 1), result.GetAllocator());
		}
	}

	return root;
}

/*****************************************************************************/

// Private and called internally by ZvalToJSON
Value JSON::ObjectZvalToJSON(Document &result, zval *z, int depth) {

std::cout << "Entering ObjectZvalToJSON" << std::endl;

	// Prevent us from recursing so deeply that we crash the PHP process
	if (depth > ZVAL_TO_JSON_MAX_DEPTH) {
		throw JSONException(
			std::string("Maximum array depth of ") +
			std::to_string(ZVAL_TO_JSON_MAX_DEPTH) +
			" exceeded"
		);
	}

	// The resulting JSON Object
	Value root(kObjectType);

	// Current position in the PHP array
	HashPosition pos;

	for (
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(z), &pos);
		zend_hash_has_more_elements_ex(Z_ARRVAL_P(z), &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(z), &pos)
	) {
		Value JSONValue;

		int type;
		zval *data;

		zend_string *key;
		zend_ulong idx;

		type = zend_hash_get_current_key_ex(Z_ARRVAL_P(z), &key, &idx, &pos);
		data = zend_hash_get_current_data_ex(Z_ARRVAL_P(z), &pos);

		if (HASH_KEY_IS_STRING != type) {
			throw JSONException("Cannot convert an array with a mix of numeric and associative keys to JSON");
		}

		Value NextKey(key->val, result.GetAllocator());

		if (IS_ARRAY == Z_TYPE_P(data)) {

			if (HASH_KEY_IS_STRING == GetZvalArrayType(data)) {
				root.AddMember(NextKey, ObjectZvalToJSON(result, data, depth + 1), result.GetAllocator());
			} else {
				root.AddMember(NextKey, ArrayZvalToJSON(result, data, depth + 1), result.GetAllocator());
			}
		}

		else {
			root.AddMember(NextKey, ScalarZvalToJSON(result, data, depth + 1), result.GetAllocator());
		}
	}

	return root;
}

/*****************************************************************************/

// Private and called internally by ZvalToJSON
Value JSON::ScalarZvalToJSON(Document &result, zval *z, int depth) {

std::cout << "Entering ScalarZvalToJSON" << std::endl;

	// Prevent us from recursing so deeply that we crash the PHP process
	if (depth > ZVAL_TO_JSON_MAX_DEPTH) {
		throw JSONException(
			std::string("Maximum array depth of ") +
			std::to_string(ZVAL_TO_JSON_MAX_DEPTH) +
			" exceeded"
		);
	}

	Value JSONValue;

	switch(Z_TYPE_P(z)) {

		// Warning: this WILL break until I switch to a real JSON library
		case IS_NULL:
			JSONValue.SetNull();
			break;

		case IS_LONG:

			#ifdef ZEND_ENABLE_ZVAL_LONG64
				JSONValue.SetInt64(Z_LVAL_P(z));
			#else
				JSONValue.SetInt(Z_LVAL_P(z));
			#endif

			break;

		case IS_DOUBLE:
			JSONValue.SetDouble(Z_DVAL_P(z));
			break;

		case IS_TRUE:
			JSONValue.SetBool(true);
			break;

		case IS_FALSE:
			JSONValue.SetBool(false);
			break;

		case IS_STRING:
			JSONValue.SetString(Z_STR_P(z)->val, Z_STR_P(z)->len, result.GetAllocator());
			break;

		case IS_OBJECT:
		case IS_RESOURCE:
			throw JSONException("Cannot convert an object or a resource into a JSON value");

		default:
			throw JSONException("Encountered unknown data type in ScalarZvalToJSON");
	}

	return JSONValue;
}

/*****************************************************************************/

// Private and called internally by ZvalToJSON
Value JSON::ZvalToJSONValue(Document &result, zval *z, int depth) {

	Value root;

	if (IS_ARRAY == Z_TYPE_P(z)) {

		if (HASH_KEY_IS_STRING == GetZvalArrayType(z)) {
			root = ObjectZvalToJSON(result, z, depth);
		} else {
			root = ArrayZvalToJSON(result, z, depth);
		}
	}

	else {
		root = ScalarZvalToJSON(result, z, depth);
	}

	return root;
}

/*****************************************************************************/

std::string JSON::serialize(Document document) {

	StringBuffer buffer;

	buffer.Clear();

	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	return std::string(buffer.GetString());
}

/*****************************************************************************/

Document JSON::deserialize(std::string json) {

	Document document;
	ParseResult status = document.Parse(json.c_str());

	if (!status) {
		throw JSONException(GetParseError_En(status.Code()));
	}

	return document;
}

/*****************************************************************************/

zval JSON::JSONToZval(Value const &value) {

	if (value.IsArray()) {
		return ArrayToZval(value);
	}

	else if (value.IsObject()) {
		return ObjectToZval(value);
	}

	else {
		return ScalarToZval(value);
	}
}

/*****************************************************************************/

Document JSON::ZvalToJSON(zval *z) {

	Document result;

	result.CopyFrom(ZvalToJSONValue(result, z, 1), result.GetAllocator());
	return result;
}
