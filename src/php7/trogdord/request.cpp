#include "request.h"
#include "network/tcpconnectionmap.h"

#include "exception/networkexception.h"
#include "exception/requestexception.h"


JSONObject Request::execute(std::string hostname, unsigned short port, std::string request) {

	try {

		// As long as the connection wasn't closed at any point (either
		// intentionally or due to error), this should be fast and should only
		// retrieve an already opened and cached connection.
		TCPConnection &connection = TCPConnectionMap::get().connect(hostname, port);

		connection.write(request);
		std::string response = connection.read();

		JSONObject responseObj = JSON::deserialize(response);
		auto status = responseObj.get_optional<int>("status");

		if (!status || SUCCESS != *status) {
			auto message = responseObj.get_optional<std::string>("message");
			throw RequestException(message ? *message : "Invalid request");
		}

		return responseObj;
	}

	catch (boost::property_tree::json_parser::json_parser_error &e) {
		throw NetworkException("Invalid response");
	}
}
