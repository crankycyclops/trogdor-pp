#include <trogdor/utility.h>

#include "include/dispatcher.h"
#include "include/response.h"
#include "include/exception/requestexception.h"

#include "include/scopes/global.h"
#include "include/scopes/game.h"


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
	scopes[Global::SCOPE] = Global::get().get();
	scopes[Game::SCOPE] = Game::get().get();
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

	try {
		return strToLower(requestObj.get<std::string>(component));
	}

	catch (boost::property_tree::ptree_bad_data &e) {
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

	catch (std::exception &e) {
		throw RequestException(INVALID_JSON, 400);
	}

	method = parseRequestComponent(requestObj, METHOD);
	scope = parseRequestComponent(requestObj, SCOPE);
	action = parseRequestComponent(requestObj, ACTION);

	return requestObj;
}

/*****************************************************************************/

std::string Dispatcher::dispatch(std::string request) {

	std::string method;
	std::string scope;
	std::string action;

	try {

		JSONObject requestObj = parseRequest(request, method, scope, action);

		// Make sure the specified scope can be resolved
		if (scopes.end() == scopes.find(scope)) {
			return Response::makeErrorJson(SCOPE_NOT_FOUND, 404);
		}

		return JSON::serialize(scopes[scope]->resolve(method, action, requestObj));
	}

	catch (RequestException &e) {
		return Response::makeErrorJson(e.what(), e.getStatus());
	}
}
