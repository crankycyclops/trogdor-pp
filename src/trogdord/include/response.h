#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include "json.h"


class Response {

	public:

		// Crafts a simple JSON response that looks like this:
		// {"status":<statusCode>,"message":<message>}. This method is
		// provided as a shortcut that sidesteps the
		// serialization/deserialization steps necessary for longer responses.
		static inline std::string makeErrorJson(std::string message, int statusCode) {

			return std::string("{\"status\":") + std::to_string(statusCode) +
				",\"message\":\"" + message + "\"}";
		}
};


#endif
