#ifndef JSON_H
#define JSON_H

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


// What we get after deserializing a JSON string. Using iptree instead of ptree
// allows me to do case-insensitive searches on keys.
typedef boost::property_tree::iptree JSONObject;

// An interface for serializing and deserializing JSON objects
class JSON {

	public:

		// JSONObject -> JSON string
		static std::string serialize(JSONObject obj);

		// JSON string -> JSONObject
		static JSONObject deserialize(std::string json);
};


#endif
