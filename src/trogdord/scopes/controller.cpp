#include <optional>
#include "../include/scopes/controller.h"


const char *ScopeController::DEFAULT_ACTION = "default";

const char *ScopeController::METHOD_NOT_FOUND = "method not found";
const char *ScopeController::ACTION_NOT_FOUND = "action not found";

/*****************************************************************************/

void ScopeController::registerAction(
	std::string method,
	std::string action,
	std::function<JSONObject(JSONObject)> callback
) {

	if (actionMap.end() == actionMap.find(method)) {
		actionMap[method] = {};
	}

	actionMap[method][action] = callback;
}

/*****************************************************************************/

JSONObject ScopeController::resolve(
	std::shared_ptr<TCPConnection> &connection,
	std::string method,
	std::string action,
	JSONObject requestObj
) {

	JSONObject response;

	// If an action is missing from the request, try the default
	action = 0 == action.compare("") ? DEFAULT_ACTION : action;

	if (actionMap.end() == actionMap.find(method)) {

		response.put("status", 404);
		response.put("message", METHOD_NOT_FOUND);

		connection->log(trogdor::Trogerr::INFO, std::string("404: ") + METHOD_NOT_FOUND);

		return response;
	}

	else if (actionMap[method].end() == actionMap[method].find(action)) {

		std::string message;
		response.put("status", 404);

		if (0 == action.compare(DEFAULT_ACTION)) {
			message = std::string("no default action for method ") + method;
		} else {
			message = ACTION_NOT_FOUND;
		}

		connection->log(trogdor::Trogerr::INFO, std::string("404: ") + message);
		response.put("message", message);

		return response;
	}

	response = actionMap[method][action](requestObj);

	std::string logMessage = response.get<std::string>("status");
	boost::optional responseMessage = response.get_optional<std::string>("message");

	if (responseMessage) {
		logMessage += std::string(" ") + *responseMessage;
	}

	connection->log(trogdor::Trogerr::INFO, logMessage);
	return response;
}
