#include <regex>

#include <trogdord/filter/filter.h>
#include <trogdord/exception/filterexception.h>


Filter Filter::makeFilter(std::string type, const rapidjson::Value &value) {

	switch (value.GetType()) {

		case rapidjson::kNumberType:

			if (value.IsUint()) {
				return {type, static_cast<size_t>(value.GetUint())};
			}

			else if (value.IsInt()) {
				return {type, value.GetInt()};
			}

			else {
				return {type, value.GetDouble()};
			}

		case rapidjson::kTrueType:

			return {type, true};

		case rapidjson::kFalseType:

			return {type, false};

		case rapidjson::kStringType:

			return {type, std::string(value.GetString())};

		case rapidjson::kNullType:

			throw FilterException("null filter values are unsupported");

		case rapidjson::kArrayType:
		case rapidjson::kObjectType:

			throw FilterException("Filter values cannot contain nested objects or arrays");

		default:

			throw FilterException("unsupported filter value");

	}
}

/*****************************************************************************/

Filter::Group Filter::JSONToFilterGroup(const rapidjson::Value &json) {

	Group fg;

	if (rapidjson::kObjectType != json.GetType()) {
		throw FilterException("Filter group must be an object of key, value pairs");
	}

	for (auto i = json.MemberBegin(); i != json.MemberEnd(); i++) {

		if (!i->name.IsString()) {
			throw FilterException("Filter type must be a string");
		}

		std::string type = i->name.GetString();

		// We're parsing more than one filter of the same type
		if (rapidjson::kArrayType == i->value.GetType()) {

			for (auto j = i->value.Begin(); j != i->value.End(); j++) {
				fg.push_back(makeFilter(type, *j));
			}
		}

		// We're parsing a single filter of the given type
		else {
			fg.push_back(makeFilter(type, i->value));
		}
	}

	return fg;
}

/*****************************************************************************/

Filter::Union Filter::JSONToFilterUnion(const rapidjson::Value &json) {

	Filter::Union result;

	// We're parsing a union of filter groups
	if (json.IsArray() && json.Size()) {

		for (auto i = json.Begin(); i != json.End(); i++) {

			Group g = JSONToFilterGroup(*i);

			if (g.size()) {
				result.push_back(g);
			}
		}
	}

	// We're parsing a single filter group
	else if (json.IsObject() && json.MemberBegin() != json.MemberEnd()) {
		result.push_back(JSONToFilterGroup(json));
	}

	return result;
}
