#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>


class Response {

	public:

		// These are all the possible status codes that can be returned.
		static constexpr int STATUS_SUCCESS = 200;
		static constexpr int STATUS_PARTIAL_CONTENT = 206;
		static constexpr int STATUS_INVALID = 400;
		static constexpr int STATUS_NOT_FOUND = 404;
		static constexpr int STATUS_CONFLICT = 409;
		static constexpr int STATUS_INTERNAL_ERROR = 500;
		static constexpr int STATUS_UNSUPPORTED = 501;

		// Default generic message that should be returned when an internal
		// error occurs
		static constexpr const char *INTERNAL_ERROR_MSG = "An internal error occurred";

		// Error message when state must be enabled and it's not
		static constexpr const char *STATE_DISABLED = "operation requires state.enabled to be set to true in trogdord.ini.";

		// Crafts a simple JSON response that looks like this:
		// {"status":<statusCode>,"message":<message>}. This method is
		// provided as a shortcut that sidesteps the
		// serialization/deserialization steps necessary for longer responses.
		static inline std::string makeErrorJson(const std::string &message, const int &statusCode) {

			return std::string("{\"status\":") + std::to_string(statusCode) +
				",\"message\":\"" + message + "\"}";
		}
};


#endif
