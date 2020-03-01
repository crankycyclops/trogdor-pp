#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include "json.h"


class Request {

	public:

		// String representations of the five request methods
		static const char *GET;
		static const char *POST;
		static const char *PUT;
		static const char *SET;
		static const char *DELETE;

		// Error messages for missing or invalid arguments
		static const char *MISSING_GAME_ID;
		static const char *INVALID_GAME_ID;

		// Utility function that parses a request's argument.
		template<typename argumentType>
		static argumentType parseArgument(
			JSONObject request,
			std::string argument,
			std::string missingErrMsg,
			std::string invalidErrMsg
		) {

			try {
				return request.get<argumentType>(argument);
			}

			// It's kind of yucky catching an exception here just to throw
			// another one, but as you can see, it cleans up my code a lot.
			// Perhaps I can return a std::variant instead? I'll think about
			// this some more and revisit it.
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

		// Utility function that parses a numeric id from a request's
		// arguments.
		static inline int parseId(
			JSONObject request,
			std::string idField,
			std::string missingErrMsg,
			std::string invalidErrMsg
		) {

			int id = parseArgument<int>(request, idField, missingErrMsg, invalidErrMsg);

			if (id < 0) {

				JSONObject response;

				response.put("status", 400);
				response.put("message", invalidErrMsg);

				throw response;
			}

			return id;
		}

		// Utility function that parses a game id from a request's arguments.
		static inline int parseGameId(JSONObject request, std::string idField = "args.id") {

			return parseId(request, idField, MISSING_GAME_ID, INVALID_GAME_ID);
		}
};


#endif
