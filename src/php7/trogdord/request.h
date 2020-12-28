#ifndef PHP_REQUEST_H
#define PHP_REQUEST_H

#include "json.h"


class Request {

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
