#include "../include/scopes/controller.h"


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
	std::string method,
	std::string action,
	JSONObject requestObj
) {

	JSONObject response;

	if (actionMap.end() == actionMap.find(method)) {

		response.put("status", 404);
		response.put("message", METHOD_NOT_FOUND);

		return response;
	}

	else if (actionMap[method].end() == actionMap[method].find(action)) {

		response.put("status", 404);
		response.put("message", ACTION_NOT_FOUND);

		return response;
	}

	return actionMap[method][action](requestObj);
}
