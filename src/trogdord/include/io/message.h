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

		// Converts Message to a JSON object
		inline JSONObject toJSONObject() {

			JSONObject o;

			o.put("timestamp", timestamp);
			o.put("content", content);

			return o;
		}
	};
}


#endif
