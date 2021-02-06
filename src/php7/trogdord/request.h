#ifndef PHP_REQUEST_H
#define PHP_REQUEST_H

#include "json.h"


class Request {

	private:

		// Sets the status property on the trogdord object after a response is received.
		static void setStatusProperty(zval *trogdord, uint status);

	public:

		// Sends a request to trogdord and processes the response.
		static Document execute(
			std::string hostname,
			unsigned short port,
			std::string request,
			zval *trogdord
		);
};


#endif
