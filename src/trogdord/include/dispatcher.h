#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <memory>
#include <unordered_map>

#include "json.h"
#include "scopes/controller.h"


/*
	Dispatches JSON requests. The format, detailed below, is based heavily
	on HTTP REST.

	request JSON = {
		// This is analagous to the HTTP methods GET, PUT, POST, etc, and they
		// basically mean the same thing. "get" requests retrieve data, "post"
		// requests create data, "put" requests replacing existing data,
		// "patch" requests modify existing data, and "delete" requests, of
		// course, delete existing data.
		"method": "get" | "post" | "put" | "patch" | delete",

		// If this were an MVC web application, then the scope would be your
		// controller. Requests that have an effect on the server environment
		// as a whole fall under the "global" scope and game-specific
		// requests fall under the "game" scope. The "entity" scope is for
		// requests that are common to all entities within a game, and for
		// requests that are appropriate for more specific types of entities,
		// there are scopes for all the other entity types: "place", "room",
		// "thing", "object", "being", "creature", and "player."
		"scope": "global" | "game" | "entity" | ... all other entity types ...,

		// The action specifies what you're trying to do in the given scope.
		// This is exactly like the action in an MVC controller.
		// For example, you might send the following request, which returns
		// statistical information about the sever environment such as the
		// number of running games:
		// {"method": "get", "scope": "global", "action": "statistics"}
		"action": <action name>,

		// This is where you pass in any arguments that are required by your
		// specific request. Some requests require no arguments. In that case,
		// you can just pass in an "args" key with an empty object, or even
		// feel free to leave it out completely. Here's an example request
		// which returns details about a specific game (in this case, the
		// only required argument is the game's numeric id):
		// {"method": "get", "scope": "game", "action": "info", "args": {"id": 1}}
		"args": {...}
	}

	The following rules govern case sensitivity of request keys and values:

	1) All keys are case INSENSITIVE. This means that {"scope": ...} is
	equivalent to {"SCOPE": ...}.

	2) The following values are also case INSENSITIVE: method, scope, and
	action. That means, for example, that {"method": "get"} is equivalent to
	{"method": "GET"}, that {"scope": "game"} is equivalent to {"scope": "GAME"},
	and that {"action": "myaction"} is equivalent to {"action": "MYACTION"}.

	3) All other values are case SENSITIVE. That means, for example, that
	{"args": {"name": "John"}} is NOT equivalent to {"name": "john"}. They are
	considered different values.
*/
class Dispatcher {

	private:

		// Message to send back to the user if their request wasn't a valid
		// JSON string
		const char *INVALID_JSON_MSG = "request must be valid JSON";

		// Message to send back to the user if they didn't include a
		// method with their request
		const char *MISSING_METHOD_MSG = "missing required method";

		// Message to send back to the user if we couldn't resolve the method
		// specified in the request
		const char *INVALID_METHOD_MSG = "invalid method";

		// Message to send back to the user if they didn't include a
		// scope with their request
		const char *MISSING_SCOPE_MSG = "missing required scope";

		// Message to send back to the user if we couldn't resolve the scope
		// specified in the request
		const char *INVALID_SCOPE_MSG = "invalid scope";

		// Message to send back to the user if they didn't include an
		// action with their request
		const char *MISSING_ACTION_MSG = "missing required action";

		// Message to send back to the user if we couldn't resolve the action
		// specified in the request
		const char *INVALID_ACTION_MSG = "invalid action";

		// Singleton instance of Dispatcher
		static std::unique_ptr<Dispatcher> instance;

		// Maps scope name -> scope controller
		std::unordered_map<std::string, std::unique_ptr<ScopeController> &> scopes;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		Dispatcher();
		Dispatcher(const Dispatcher &) = delete;

		// Parses a JSON request and returns a JSONObject representation of
		// it. If there are any errors during parsing, this will throw an
		// instance of RequestException. Takes as input references to strings
		// for the method, scope, and action so we can use those values in
		// dispatch() without having to get them from the ptree again.
		JSONObject parseRequest(
			std::string request,
			std::string &method,
			std::string &scope,
			std::string &action
		);

	public:

		// Returns singleton instance of Dispatcher.
		static std::unique_ptr<Dispatcher> &get();

		// Dispatches a request and returns the server's response in JSON.
		std::string dispatch(std::string request);

		// Crafts a simple JSON response that looks like this:
		// {"status":<statusCode>,"message":<message>}
		inline std::string makeErrorJson(std::string message, int statusCode) {

			return std::string("{\"status\":") + std::to_string(statusCode) +
				",\"message\":\"" + message + "\"}";
		}
};


#endif
