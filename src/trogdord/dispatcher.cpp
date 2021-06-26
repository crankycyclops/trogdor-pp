#include <trogdor/utility.h>

#include <trogdord/json.h>
#include <trogdord/config.h>

#include <trogdord/dispatcher.h>
#include <trogdord/response.h>
#include <trogdord/exception/requestexception.h>

#include <trogdord/scopes/global.h>
#include <trogdord/scopes/game.h>
#include <trogdord/scopes/resource.h>
#include <trogdord/scopes/room.h>
#include <trogdord/scopes/object.h>
#include <trogdord/scopes/creature.h>
#include <trogdord/scopes/player.h>


// Singleton instance of Dispatcher
std::unique_ptr<Dispatcher> Dispatcher::instance = nullptr;

/*****************************************************************************/

Dispatcher::Dispatcher() {

	// Register available scopes
	scopes[GlobalController::get()->getName()] = GlobalController::get().get();
	scopes[GameController::get()->getName()] = GameController::get().get();
	scopes[EntityController::get()->getName()] = EntityController::get().get();
	scopes[ResourceController::get()->getName()] = ResourceController::get().get();
	scopes[TangibleController::get()->getName()] = TangibleController::get().get();
	scopes[PlaceController::get()->getName()] = PlaceController::get().get();
	scopes[RoomController::get()->getName()] = RoomController::get().get();
	scopes[ThingController::get()->getName()] = ThingController::get().get();
	scopes[ObjectController::get()->getName()] = ObjectController::get().get();
	scopes[BeingController::get()->getName()] = BeingController::get().get();
	scopes[CreatureController::get()->getName()] = CreatureController::get().get();
	scopes[PlayerController::get()->getName()] = PlayerController::get().get();

	// Built-in scopes cannot be unregistered later
	builtinScopes.insert(GlobalController::get()->getName());
	builtinScopes.insert(GameController::get()->getName());
	builtinScopes.insert(EntityController::get()->getName());
	builtinScopes.insert(ResourceController::get()->getName());
	builtinScopes.insert(TangibleController::get()->getName());
	builtinScopes.insert(PlaceController::get()->getName());
	builtinScopes.insert(RoomController::get()->getName());
	builtinScopes.insert(ThingController::get()->getName());
	builtinScopes.insert(ObjectController::get()->getName());
	builtinScopes.insert(BeingController::get()->getName());
	builtinScopes.insert(CreatureController::get()->getName());
	builtinScopes.insert(PlayerController::get()->getName());
}

/*****************************************************************************/

std::unique_ptr<Dispatcher> &Dispatcher::get() {

	if (!instance) {
		instance = std::unique_ptr<Dispatcher>(new Dispatcher());
	}

	return instance;
}

/*****************************************************************************/

bool Dispatcher::registerScope(ScopeController *scope) {

	if (scopes.end() != scopes.find(scope->getName())) {
		return false;
	}

	scopes[scope->getName()] = scope;
	return true;
}

/*****************************************************************************/

bool Dispatcher::unregisterScope(std::string name) {

	if (scopes.end() == scopes.find(name)) {
		return false;
	}

	else if (builtinScopes.end() != builtinScopes.find(name)) {
		return false;
	}

	scopes.erase(name);
	return true;
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
