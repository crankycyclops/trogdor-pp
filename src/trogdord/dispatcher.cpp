#include <trogdor/utility.h>

#include "include/config.h"

#include "include/dispatcher.h"
#include "include/response.h"
#include "include/exception/requestexception.h"

#include "include/scopes/global.h"
#include "include/scopes/game.h"
#include "include/scopes/player.h"


// String representations of each request component
const char *Dispatcher::METHOD = "method";
const char *Dispatcher::SCOPE = "scope";
const char *Dispatcher::ACTION = "action";
const char *Dispatcher::ARGS = "args";

// Error messages for malformatted requests
const char *Dispatcher::INVALID_JSON = "request must be valid JSON";
const char *Dispatcher::MISSING_METHOD = "missing required method";
const char *Dispatcher::INVALID_METHOD = "invalid method";
const char *Dispatcher::MISSING_SCOPE = "missing required scope";
const char *Dispatcher::INVALID_SCOPE = "invalid scope";
const char *Dispatcher::SCOPE_NOT_FOUND = "scope not found";
const char *Dispatcher::MISSING_ACTION = "missing required action";
const char *Dispatcher::INVALID_ACTION = "invalid action";

// Singleton instance of Dispatcher
std::unique_ptr<Dispatcher> Dispatcher::instance = nullptr;

/*****************************************************************************/

Dispatcher::Dispatcher() {

	// Register available scopes
	scopes[GlobalController::SCOPE] = GlobalController::get().get();
	scopes[GameController::SCOPE] = GameController::get().get();
	scopes[EntityController::SCOPE] = EntityController::get().get();
	scopes[ThingController::SCOPE] = ThingController::get().get();
	scopes[BeingController::SCOPE] = BeingController::get().get();
	scopes[PlayerController::SCOPE] = PlayerController::get().get();
}

/*****************************************************************************/

std::unique_ptr<Dispatcher> &Dispatcher::get() {

	if (!instance) {
		instance = std::unique_ptr<Dispatcher>(new Dispatcher());
	}

	return instance;
}

/*****************************************************************************/

std::string Dispatcher::parseRequestComponent(JSONObject requestObj, std::string component) {

	static std::unordered_map<std::string, bool> required = {
		{METHOD, true},
		{SCOPE, true},
		{ACTION, false}
	};

	static std::unordered_map<std::string, std::string> missingMsgs = {
		{METHOD, MISSING_METHOD},
		{SCOPE, MISSING_SCOPE},
		{ACTION, MISSING_ACTION}
	};

	static std::unordered_map<std::string, std::string> invalidMsgs = {
		{METHOD, INVALID_METHOD},
		{SCOPE, INVALID_SCOPE},
		{ACTION, INVALID_ACTION}
	};

	if (requestObj.not_found() == requestObj.find(component)) {
		if (required[component]) {
			throw RequestException(missingMsgs[component], 400);
		} else {
			return "";
		}
	}

	boost::optional value = requestObj.get_optional<std::string>(component);

	if (value) {
		return strToLower(*value);
	} else {
		throw RequestException(invalidMsgs[component], 400);
	}
}

/*****************************************************************************/

JSONObject Dispatcher::parseRequest(
	std::string request,
	std::string &method,
	std::string &scope,
	std::string &action
) {
	JSONObject requestObj;

	// Construct an object based on the request
	try {
		requestObj = JSON::deserialize(request);
	}

	catch (boost::property_tree::json_parser::json_parser_error &e) {
		throw RequestException(INVALID_JSON, 400);
	}

	method = parseRequestComponent(requestObj, METHOD);
	scope = parseRequestComponent(requestObj, SCOPE);
	action = parseRequestComponent(requestObj, ACTION);

	return requestObj;
}

/*****************************************************************************/

std::string Dispatcher::dispatch(std::shared_ptr<TCPConnection> &connection, std::string request) {

	std::string method;
	std::string scope;
	std::string action;

	// Log all incoming requests
	connection->log(trogdor::Trogerr::INFO, std::string("request: ") + trim(request));

	try {

		JSONObject requestObj = parseRequest(request, method, scope, action);

		// Make sure the specified scope can be resolved
		if (scopes.end() == scopes.find(scope)) {
			connection->log(trogdor::Trogerr::INFO, std::string("404: ") + SCOPE_NOT_FOUND);
			return Response::makeErrorJson(SCOPE_NOT_FOUND, 404);
		}

		return JSON::serialize(scopes[scope]->resolve(connection, method, action, requestObj));
	}

	catch (RequestException &e) {
		connection->log(trogdor::Trogerr::INFO, std::to_string(e.getStatus()) + ": " + e.what());
		return Response::makeErrorJson(e.what(), e.getStatus());
	}
}
