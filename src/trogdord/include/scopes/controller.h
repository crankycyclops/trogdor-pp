#ifndef SCOPE_CONTROLLER_H
#define SCOPE_CONTROLLER_H


#include <string>
#include <functional>
#include <unordered_map>
#include <memory>

#include "../json.h"


class ScopeController {

	protected:

		// If an action isn't specified in the request, we try to resolve this
		// one instead.
		static const char *DEFAULT_ACTION;

		// Error messages for malformatted requests
		static const char *METHOD_NOT_FOUND;
		static const char *ACTION_NOT_FOUND;

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
			std::string method,
			std::string action,
			std::function<JSONObject(JSONObject)> callback
		);

	public:

		// Resolves a request and returns a JSON response.
		JSONObject resolve(
			std::string method,
			std::string action,
			JSONObject requestObj
		);
};


#endif
