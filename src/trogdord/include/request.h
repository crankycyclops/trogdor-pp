#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include "json.h"


class Request {

	public:

		// String representations of the five request methods
		static const char *GET;
		static const char *POST;
		static const char *PUT;
		static const char *SET;
		static const char *DELETE;
};


#endif
