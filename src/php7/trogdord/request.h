#ifndef PHP_REQUEST_H
#define PHP_REQUEST_H

#include "json.h"


class Request {

	public:

		// This status returned as part of the response indicates that the
		// request was successful.
		static const int SUCCESS = 200;

		// Sends a request to trogdord and processes the response.
		static Document execute(
			std::string hostname,
			unsigned short port,
			std::string request
		);
};


#endif
