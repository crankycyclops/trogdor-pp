#include "request.h"
#include "network/tcpconnectionmap.h"

#include "exception/jsonexception.h"
#include "exception/networkexception.h"
#include "exception/requestexception.h"


Document Request::execute(std::string hostname, unsigned short port, std::string request) {

	try {

		// As long as the connection wasn't closed at any point (either
		// intentionally or due to error), this should be fast and should only
		// retrieve an already opened and cached connection.
		TCPConnection &connection = TCPConnectionMap::get().connect(hostname, port);

		connection.write(request);
		std::string response = connection.read();

		Document responseObj = JSON::deserialize(response);

		uint status = RequestException::DEFAULT_CODE;

		if (!responseObj.HasMember("status") || SUCCESS != (status = responseObj["status"].GetUint())) {
			throw RequestException(
				responseObj.HasMember("message") ? responseObj["message"].GetString() : "Invalid request",
				status
			);
		}

		return responseObj;
	}

	catch (const JSONException &e) {
		throw NetworkException("Invalid response");
	}
}
