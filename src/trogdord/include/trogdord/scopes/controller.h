#ifndef SCOPE_CONTROLLER_H
#define SCOPE_CONTROLLER_H


#include <string>
#include <functional>
#include <unordered_map>
#include <memory>

#include <rapidjson/document.h>

#include "../network/tcpconnection.h"


class ScopeController {

	protected:

		// Maps method, action pairs to methods in classes that inherit from
		// ScopeCotroller.
		std::unordered_map<
			std::string,
			std::unordered_map<
				std::string,
				std::function<rapidjson::Document(const rapidjson::Document &)>
			>
		> actionMap;

		// Maps method, action pairs to callback functions that invoke and
		// return the values of methods in classes that inherit from
		// ScopeController.
		void registerAction(
			const std::string &method,
			const std::string &action,
			const std::function<rapidjson::Document(const rapidjson::Document &)> &callback
		);

	public:

		// Error messages for malformatted requests.
		static constexpr const char *METHOD_NOT_FOUND = "method not found";
		static constexpr const char *ACTION_NOT_FOUND = "action not found";
		static constexpr const char *INVALID_ARGUMENTS = "args must be a valid JSON object";

		// Error messages that are common to more than one scope controller.
		static constexpr const char *GAME_NOT_FOUND = "game not found";

		// If an action isn't specified in the request, we try to resolve this
		// one instead.
		static constexpr const char *DEFAULT_ACTION = "default";

		// Returns the scope's name
		virtual const char *getName() = 0;

		// Resolves a request and returns a JSON response.
		rapidjson::Document resolve(
			std::shared_ptr<TCPConnection> &connection,
			std::string method,
			std::string action,
			const rapidjson::Document &requestObj
		);
};


#endif
