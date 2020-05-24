#include <regex>

#include "../include/filter/filter.h"
#include "../include/exception/filterexception.h"


Filter Filter::makeFilter(std::string type, std::string value) {

	static std::regex positiveInt("[+]?[0-9]+");

	if (std::regex_match(value, positiveInt)) {

		size_t intVal;
		std::stringstream sstream(value);

		sstream >> intVal;
		return {type, intVal};
	}

	// Boolean detection is EXTREMELY error prone (it's very plausible that
	// I'll read an intended string value of "true" or "false" and get the
	// wrong type as a result.) For now, there's not much I can do, but in the
	// long term, I'm going to have to replace boost::ptree with a real JSON
	// library.
	else if (0 == value.compare("true")) {
		return {type, true};
	}

	else if (0 == value.compare("false")) {
		return {type, false};
	}

	else {
		return {type, value};
	}
}

/*****************************************************************************/

Filter::Group Filter::JSONToFilterGroup(JSONObject json) {

	Group fg;

	if (json.size()) {

		if (0 == json.begin()->first.compare("")) {
			throw FilterException("Filter group must be an object of key, value pairs");
		}

		else {

			for (auto &pair: json) {

				std::string type = pair.first;

				// We're parsing more than one filter of the same type
				if (!pair.second.empty() && pair.second.data().empty()) {

					for (auto &val: pair.second) {

						if (!val.second.empty()) {
							throw FilterException("Filter values cannot contain nested arrays");
						} else {
							fg.push_back(makeFilter(type, val.second.data()));
						}
					}
				}

				// We're parsing a single filter of the given type
				else {
					fg.push_back(makeFilter(type, pair.second.data()));
				}
			}
		}
	}

	return fg;
}

/*****************************************************************************/

Filter::Union Filter::JSONToFilterUnion(JSONObject json) {

	Filter::Union result;

	if (json.size()) {

		// We're parsing a union of filter groups
		if (0 == json.begin()->first.compare("")) {

			for (auto &pair: json) {

				Group g = JSONToFilterGroup(pair.second);

				if (g.size()) {
					result.push_back(g);
				}
			}
		}

		// We're parsing a single filter group
		else {
			result.push_back(JSONToFilterGroup(json));
		}
	}

	return result;
}
