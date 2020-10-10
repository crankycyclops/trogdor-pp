#include <optional>

#include "../include/json.h"
#include "../include/response.h"
#include "../include/scopes/controller.h"


const char *ScopeController::DEFAULT_ACTION = "default";

const char *ScopeController::METHOD_NOT_FOUND = "method not found";
const char *ScopeController::ACTION_NOT_FOUND = "action not found";
const char *ScopeController::GAME_NOT_FOUND = "game not found";

/*****************************************************************************/

void ScopeController::registerAction(
	const std::string &method,
	const std::string &action,
	const std::function<rapidjson::Document(const rapidjson::Document &)> &callback
) {

	if (actionMap.end() == actionMap.find(method)) {
		actionMap[method] = {};
	}

	actionMap[method][action] = callback;
}

/*****************************************************************************/

rapidjson::Document ScopeController::resolve(
	std::shared_ptr<TCPConnection> &connection,
	std::string method,
	std::string action,
	const rapidjson::Document &requestObj
) {

	rapidjson::Document response(rapidjson::kObjectType);

	// If an action is missing from the request, try the default
	action = 0 == action.compare("") ? DEFAULT_ACTION : action;

	if (actionMap.end() == actionMap.find(method)) {

		response.AddMember(
			"status",
			rapidjson::Value().SetInt(Response::STATUS_NOT_FOUND),
			response.GetAllocator()
		);

		response.AddMember(
			"message",
			rapidjson::Value().SetString(rapidjson::StringRef(METHOD_NOT_FOUND)),
			response.GetAllocator()
		);

		if (connection) {
			connection->log(trogdor::Trogerr::INFO, std::string("404: ") + METHOD_NOT_FOUND);
		}

		return response;
	}

	else if (actionMap[method].end() == actionMap[method].find(action)) {

		std::string message;

		response.AddMember(
			"status",
			rapidjson::Value().SetInt(Response::STATUS_NOT_FOUND),
			response.GetAllocator()
		);

		if (0 == action.compare(DEFAULT_ACTION)) {
			message = std::string("no default action for method ") + method;
		} else {
			message = ACTION_NOT_FOUND;
		}

		if (connection) {
			connection->log(trogdor::Trogerr::INFO, std::string("404: ") + message);
		}

		response.AddMember(
			"message",
			rapidjson::Value().SetString(rapidjson::StringRef(message.c_str())),
			response.GetAllocator()
		);

		return response;
	}

	response = actionMap[method][action](requestObj);

	std::optional<std::string> responseMessage;
	std::string logMessage = std::to_string(response["status"].GetInt());

	if (response.HasMember("message") && response["status"].IsString()) {
		responseMessage = response["status"].GetString();
	}

	if (responseMessage) {
		logMessage += std::string(" ") + *responseMessage;
	}

	if (connection) {
		connection->log(trogdor::Trogerr::INFO, logMessage);
	}

	return response;
}
