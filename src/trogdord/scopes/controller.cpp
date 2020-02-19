#include <optional>
#include "../include/scopes/controller.h"


const char *ScopeController::DEFAULT_ACTION = "default";

const char *ScopeController::METHOD_NOT_FOUND = "method not found";
const char *ScopeController::ACTION_NOT_FOUND = "action not found";

const char *ScopeController::MISSING_GAME_ID = "missing required game id";
const char *ScopeController::INVALID_GAME_ID = "invalid game id";

/*****************************************************************************/

int ScopeController::parseGameId(JSONObject request, std::string idField) {

	try {

		int gameId = request.get<int>(idField);

		if (gameId < 0) {

			JSONObject response;

			response.put("status", 400);
			response.put("message", INVALID_GAME_ID);

			throw response;
		}

		return gameId;
	}

	// It's kind of yucky catching an exception here just to throw another
	// one, but as you can see, in methods like getGame, it cleans up my code
	// a lot. Perhaps I can return a std::variant instead? I'll think about
	// this some more and revisit it.
	catch (boost::property_tree::ptree_bad_path &e) {

		JSONObject response;

		response.put("status", 400);
		response.put("message", MISSING_GAME_ID);

		throw response;
	}

	catch (boost::property_tree::ptree_bad_data &e) {

		JSONObject response;

		response.put("status", 400);
		response.put("message", INVALID_GAME_ID);

		throw response;
	}
}

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
