#ifndef REQUEST_EXCEPTION_H
#define REQUEST_EXCEPTION_H


#include "serverexception.h"


class RequestException: public ServerException {

	protected:

		// Error status code that we should return along with the error
		unsigned int code;

	public:

		// Contructors
		explicit RequestException(const std::string &what_arg);
		explicit RequestException(const char *what_arg);
		explicit RequestException(const std::string &what_arg, unsigned int code);
		explicit RequestException(const char *what_arg, unsigned int code);

		// Returns the response status code
		inline unsigned int getStatus() {return code;}
};


#endif
