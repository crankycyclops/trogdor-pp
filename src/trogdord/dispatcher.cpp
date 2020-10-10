#include <trogdor/utility.h>

#include "include/json.h"
#include "include/config.h"

#include "include/dispatcher.h"
#include "include/response.h"
#include "include/exception/requestexception.h"

#include "include/scopes/global.h"
#include "include/scopes/game.h"
#include "include/scopes/resource.h"
#include "include/scopes/room.h"
#include "include/scopes/object.h"
#include "include/scopes/creature.h"
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
	scopes[ResourceController::SCOPE] = ResourceController::get().get();
	scopes[TangibleController::SCOPE] = TangibleController::get().get();
	scopes[PlaceController::SCOPE] = PlaceController::get().get();
	scopes[RoomController::SCOPE] = RoomController::get().get();
	scopes[ThingController::SCOPE] = ThingController::get().get();
	scopes[ObjectController::SCOPE] = ObjectController::get().get();
	scopes[BeingController::SCOPE] = BeingController::get().get();
	scopes[CreatureController::SCOPE] = CreatureController::get().get();
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

std::string Dispatcher::parseRequestComponent(
	const rapidjson::Document &requestObj,
	std::string component
) {

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

	if (!requestObj.HasMember(component.c_str())) {
		if (required[component]) {
			throw RequestException(missingMsgs[component], Response::STATUS_INVALID);
		} else {
			return "";
		}
	}

	else if (!requestObj[component.c_str()].IsString()) {
		throw RequestException(invalidMsgs[component], Response::STATUS_INVALID);
	}

	return trogdor::strToLower(requestObj[component.c_str()].GetString());
}

/*****************************************************************************/

rapidjson::Document Dispatcher::parseRequest(
	std::string request,
	std::string &method,
	std::string &scope,
	std::string &action
) {
	rapidjson::Document requestObj;
	requestObj.Parse(request.c_str());

	if (requestObj.HasParseError()) {
		throw RequestException(INVALID_JSON, Response::STATUS_INVALID);
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
	if (connection) {
		connection->log(trogdor::Trogerr::INFO, std::string("request: ") + trogdor::trim(request));
	}

	try {

		rapidjson::Document requestObj = parseRequest(request, method, scope, action);

		// Make sure the specified scope can be resolved
		if (scopes.end() == scopes.find(scope)) {

			if (connection) {
				connection->log(trogdor::Trogerr::INFO, std::string("404: ") + SCOPE_NOT_FOUND);
			}

			return Response::makeErrorJson(SCOPE_NOT_FOUND, Response::STATUS_NOT_FOUND);
		}

		return JSON::serialize(scopes[scope]->resolve(connection, method, action, requestObj));
	}

	catch (RequestException &e) {

		if (connection) {
			connection->log(trogdor::Trogerr::INFO, std::to_string(e.getStatus()) + ": " + e.what());
		}

		return Response::makeErrorJson(e.what(), e.getStatus());
	}
}
