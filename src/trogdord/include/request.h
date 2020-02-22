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
		static const char *MISSING_PLAYER_ID;
		static const char *INVALID_PLAYER_ID;

		// Utility function that parses an id from a request's arguments.
		static int parseId(
			JSONObject request,
			std::string idField,
			std::string missingErrMsg,
			std::string invalidErrMsg
		);

		// Utility function that parses a game id from a request's arguments.
		static inline int parseGameId(JSONObject request, std::string idField = "args.id") {

			return parseId(request, idField, MISSING_GAME_ID, INVALID_GAME_ID);
		}

		// Utility function that parses a player id from a request's arguments.
		static inline int parsePlayerId(JSONObject request, std::string idField = "args.id") {

			return parseId(request, idField, MISSING_PLAYER_ID, INVALID_PLAYER_ID);
		}
};


#endif
