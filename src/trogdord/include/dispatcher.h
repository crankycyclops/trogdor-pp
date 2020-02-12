#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <memory>
#include <unordered_map>

#include "json.h"
#include "scopes/controller.h"
#include "network/tcpconnection.h"


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

	Some method/scope pairs define a default action, allowing you to leave the
	"action" parameter out of the request. For example, to get the details of a
	running game with id 1, you would send the following request:
	{"method":"get","scope":"game","args":{"id":1}}

	If for some reason you want to call the default action explicitly, the
	following request would also work:
	{"method":"get","scope":"game","action":"default","args":{"id":1}}

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

		// Error messages for malformatted requests
		static const char *INVALID_JSON;
		static const char *MISSING_METHOD;
		static const char *INVALID_METHOD;
		static const char *MISSING_SCOPE;
		static const char *INVALID_SCOPE;
		static const char *SCOPE_NOT_FOUND;
		static const char *MISSING_ACTION;
		static const char *INVALID_ACTION;

		// Singleton instance of Dispatcher
		static std::unique_ptr<Dispatcher> instance;

		// Maps scope name -> scope controller
		std::unordered_map<std::string, ScopeController *> scopes;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		Dispatcher();
		Dispatcher(const Dispatcher &) = delete;

		std::string parseRequestComponent(JSONObject requestObj, std::string component);

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

		// The four possible components of a request.
		static const char *METHOD;
		static const char *SCOPE;
		static const char *ACTION;
		static const char *ARGS;

		// Returns singleton instance of Dispatcher.
		static std::unique_ptr<Dispatcher> &get();

		// Dispatches a request and returns the server's response in JSON.
		std::string dispatch(std::shared_ptr<TCPConnection> &connection, std::string request);
};


#endif
