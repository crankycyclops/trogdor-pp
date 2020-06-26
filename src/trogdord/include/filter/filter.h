#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <variant>
#include <vector>

#include "../json.h"


/*
   A Filter is used to return a set of things that match a specific criterion.
   For example, a filter with the name "is_running" that takes a boolean value
   could be used to return a set of all games that are either running or not
   running.
*/
class Filter {

	public:

		// A filter group represents the intersection of one or more filters
		// (in the case of one, the filter group is equivalent to the filter.)
		typedef std::vector<Filter> Group;

		// This is the union of one or more filter groups (in the case of one,
		// the filter set is equivalent to the filter group.)
		typedef std::vector<Group> Union;

	private:

		typedef std::variant<std::string, bool, size_t> FilterValue;

		std::string type;
		FilterValue value;

		// Takes as input a type and value and returns a filter (only called
		// internally.) This method will make sure the value is parsed into
		// the appropriate type.
		static Filter makeFilter(std::string type, std::string value);

		// Parses a filter group out of a JSONObject
		static Group JSONToFilterGroup(JSONObject json);

	public:

		// Constructor
		// Note: cppcheck will suggest you pass these arguments by reference,
		// but doing that will result in a failed build.
		inline Filter(std::string type, FilterValue value):
			type(type), value(value) {};

		// Returns the filter's type
		inline const std::string getType() const {return type;}

		// Returns the filter's value
		template<typename type>
		const type getValue() const {return std::get<type>(value);}

		// Returns the filter's value type
		size_t getValueType() const {return value.index();}

		// Takes as input a JSONObject and returns the equivalent Union of
		// filters
		static Union JSONToFilterUnion(JSONObject json);
};

#ifdef DEBUG

	#include <iostream>

	// Allow Filters to be printed via cout
	inline std::ostream &operator<<(std::ostream &o, Filter const &f) {

		o << "\t\t{Type: " << f.getType() << ", Value: ";

		if (0 == f.getValueType()) {
			o << f.getValue<std::string>() << " (string)";
		}

		else if (1 == f.getValueType()) {
			o << (f.getValue<bool>() ? "true" : "false") << " (bool)";
		}

		else {
			o << f.getValue<size_t>() << " (size_t)";
		}

		o << "}\n";

		return o;
	}

	/************************************************************************/

	// Allow Filter::Group to be printed via cout
	inline std::ostream &operator<<(std::ostream &o, Filter::Group const &g) {

		o << "\n\tFilter Group:\n\n";

		for (auto const &filter: g) {
			o << filter;
		};

		return o;
	}

	/************************************************************************/

	// Allow Filter::Union to be printed via cout
	inline std::ostream &operator<<(std::ostream &o, Filter::Union const &u) {

		o << "\nFilter Union:\n";

		for (auto const &group: u) {
			o << group;
		};

		return o;
	}

#endif // DEBUG


#endif // FILTER_H
