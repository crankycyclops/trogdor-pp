#ifndef OUTPUT_MESSAGE_H
#define OUTPUT_MESSAGE_H

#include <string>
#include <ctime>

#include "../json.h"

namespace output {


	// Represents a single message to be inserted into the output buffer.
	struct Message {

		time_t timestamp;
		std::string content;
		size_t order = 0;

		// Converts Message to a JSON object
		rapidjson::Document toJSONObject() {

			rapidjson::Document o;

			o.AddMember("timestamp", timestamp, o.GetAllocator());
			o.AddMember("content", rapidjson::StringRef(content.c_str()), o.GetAllocator());
			o.AddMember("order", order, o.GetAllocator());

			return o;
		}
	};
}


#endif
