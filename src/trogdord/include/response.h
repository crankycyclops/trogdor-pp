#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include "json.h"


class Response {

	public:

		// These are all the possible status codes that can be returned.
		const static int STATUS_SUCCESS = 200;
		const static int STATUS_INVALID = 400;
		const static int STATUS_NOT_FOUND = 404;
		const static int STATUS_CONFLICT = 409;
		const static int STATUS_INTERNAL_ERROR = 500;
		const static int STATUS_UNSUPPORTED = 501;

		// Crafts a simple JSON response that looks like this:
		// {"status":<statusCode>,"message":<message>}. This method is
		// provided as a shortcut that sidesteps the
		// serialization/deserialization steps necessary for longer responses.
		// Note: cppcheck will warn that you should pass message by reference,
		// but this is bad advice and should be ignored.
		static inline std::string makeErrorJson(std::string message, int statusCode) {

			return std::string("{\"status\":") + std::to_string(statusCode) +
				",\"message\":\"" + message + "\"}";
		}
};


#endif
