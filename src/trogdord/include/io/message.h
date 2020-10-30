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
		rapidjson::Value toJSONObject(rapidjson::MemoryPoolAllocator<> &allocator) {

			rapidjson::Value o(rapidjson::kObjectType);
			rapidjson::Value val(rapidjson::kStringType);

			val.SetString(rapidjson::StringRef(content.c_str()), allocator);

			o.AddMember("timestamp", timestamp, allocator);
			o.AddMember("content", val, allocator);
			o.AddMember("order", order, allocator);

			return o;
		}
	};
}


#endif
