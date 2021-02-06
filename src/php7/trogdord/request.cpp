#include "request.h"
#include "response.h"
#include "trogdord.h"
#include "network/tcpconnectionmap.h"

#include "exception/jsonexception.h"
#include "exception/networkexception.h"
#include "exception/requestexception.h"

ZEND_EXTERN_MODULE_GLOBALS(trogdord);


void Request::setStatusProperty(zval *trogdord, uint status) {

	ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord))->data.unlockStatusProperty = true;

	zend_update_property_long(
		TROGDORD_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(trogdord),
		#else
			trogdord,
		#endif
		STATUS_PROPERTY,
		strlen(STATUS_PROPERTY),
		status
	);

	ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord))->data.unlockStatusProperty = false;
}

/*****************************************************************************/

Document Request::execute(
	std::string hostname,
	unsigned short port,
	std::string request,
	zval *trogdord
) {

	try {

		// As long as the connection wasn't closed at any point (either
		// intentionally or due to error), this should be fast and should only
		// retrieve an already opened and cached connection.
		TCPConnection &connection = TCPConnectionMap::get().connect(hostname, port);

		connection.write(request);
		std::string response = connection.read();

		Document responseObj = JSON::deserialize(response);
		uint status = RequestException::DEFAULT_CODE;

		if (!responseObj.HasMember("status") || STATUS_SUCCESS != (status = responseObj["status"].GetUint())) {
			setStatusProperty(trogdord, status);
			throw RequestException(
				responseObj.HasMember("message") ? responseObj["message"].GetString() : "Invalid request",
				status
			);
		}

		setStatusProperty(trogdord, status);
		return responseObj;
	}

	catch (const JSONException &e) {
		throw NetworkException("Invalid response");
	}
}
