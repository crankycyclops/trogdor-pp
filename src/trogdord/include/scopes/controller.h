#ifndef SCOPE_CONTROLLER_H
#define SCOPE_CONTROLLER_H


#include <string>
#include <functional>
#include <unordered_map>
#include <memory>

#include "../json.h"
#include "../network/tcpconnection.h"


class ScopeController {

	protected:

		// Error messages for malformatted requests.
		static const char *METHOD_NOT_FOUND;
		static const char *ACTION_NOT_FOUND;

		// Error messages that are common to more than one scope controller.
		static const char *GAME_NOT_FOUND;

		// Maps method, action pairs to methods in classes that inherit from
		// ScopeCotroller.
		std::unordered_map<
			std::string,
			std::unordered_map<
				std::string,
				std::function<JSONObject(JSONObject)>
			>
		> actionMap;

		// Maps method, action pairs to callback functions that invoke and
		// return the values of methods in classes that inherit from
		// ScopeController.
		void registerAction(
			const std::string &method,
			const std::string &action,
			const std::function<JSONObject(JSONObject)> &callback
		);

	public:

		// If an action isn't specified in the request, we try to resolve this
		// one instead.
		static const char *DEFAULT_ACTION;

		// Resolves a request and returns a JSON response.
		JSONObject resolve(
			std::shared_ptr<TCPConnection> &connection,
			std::string method,
			std::string action,
			JSONObject requestObj
		);
};


#endif
