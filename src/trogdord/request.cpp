#include "include/request.h"


// String representations of the five request methods
const char *Request::GET = "get";
const char *Request::POST = "post";
const char *Request::PUT = "put";
const char *Request::SET = "set";
const char *Request::DELETE = "delete";

// Error messages for missing or invalid arguments
const char *Request::MISSING_GAME_ID = "missing required game id";
const char *Request::INVALID_GAME_ID = "invalid game id";
const char *Request::MISSING_PLAYER_ID = "missing required player id";
const char *Request::INVALID_PLAYER_ID = "invalid player id";

/*****************************************************************************/

int Request::parseId(
	JSONObject request,
	std::string idField,
	std::string missingErrMsg,
	std::string invalidErrMsg
) {

	try {

		int id = request.get<int>(idField);

		if (id < 0) {

			JSONObject response;

			response.put("status", 400);
			response.put("message", invalidErrMsg);

			throw response;
		}

		return id;
	}

	// It's kind of yucky catching an exception here just to throw another
	// one, but as you can see, it cleans up my code a lot. Perhaps I can
	// return a std::variant instead? I'll think about this some more and
	// revisit it.
	catch (boost::property_tree::ptree_bad_path &e) {

		JSONObject response;

		response.put("status", 400);
		response.put("message", missingErrMsg);

		throw response;
	}

	catch (boost::property_tree::ptree_bad_data &e) {

		JSONObject response;

		response.put("status", 400);
		response.put("message", invalidErrMsg);

		throw response;
	}
}
