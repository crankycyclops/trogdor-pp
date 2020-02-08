#include <trogdor/utility.h>

#include "include/dispatcher.h"
#include "include/exception/requestexception.h"


std::unique_ptr<Dispatcher> Dispatcher::instance = nullptr;

/*****************************************************************************/

Dispatcher::Dispatcher() {

	// TODO
}

/*****************************************************************************/

std::unique_ptr<Dispatcher> &Dispatcher::get() {

	if (!instance) {
		instance = std::unique_ptr<Dispatcher>(new Dispatcher());
	}

	return instance;
}

/*****************************************************************************/

// I'm not happy about catching and then throwing more exceptions, but for now,
// I'm going to keep this the way it is because it allows me to have cleaner
// code. Once I get this working an it's well tested, I'll revisit my use of
// boost::property_tree::iptree to see if there's a way I can do the same sort
// of error checking without having to catch exceptions.
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
		throw RequestException(INVALID_JSON_MSG, 400);
	}

	// Parse the method
	try {
		method = strToLower(requestObj.get<std::string>("method"));
	}

	catch (boost::property_tree::ptree_bad_path &e) {
		throw RequestException(MISSING_METHOD_MSG, 400);
	}

	catch (boost::property_tree::ptree_bad_data &e) {
		throw RequestException(INVALID_METHOD_MSG, 400);
	}

	// Parse the scope
	try {
		scope = strToLower(requestObj.get<std::string>("scope"));
	}

	catch (boost::property_tree::ptree_bad_path &e) {
		throw RequestException(MISSING_SCOPE_MSG, 400);
	}

	catch (boost::property_tree::ptree_bad_data &e) {
		throw RequestException(INVALID_SCOPE_MSG, 400);
	}

	// Parse the action
	try {
		action = strToLower(requestObj.get<std::string>("action"));
	}

	catch (boost::property_tree::ptree_bad_path &e) {
		throw RequestException(MISSING_ACTION_MSG, 400);
	}

	catch (boost::property_tree::ptree_bad_data &e) {
		throw RequestException(INVALID_ACTION_MSG, 400);
	}

	// Make sure the specified scope can be resolved
	if (scopes.end() == scopes.find(scope)) {
		throw RequestException(INVALID_SCOPE_MSG, 400);
	}

	return requestObj;
}

/*****************************************************************************/

std::string Dispatcher::dispatch(std::string request) {

	std::string method;
	std::string scope;
	std::string action;

	try {
		JSONObject requestObj = parseRequest(request, method, scope, action);
		// TODO: call execute method on scopes[scope], passing it requestObj
		return makeErrorJson("TODO", 200);
	}

	catch (RequestException &e) {
		return makeErrorJson(e.what(), e.getStatus());
	}
}
