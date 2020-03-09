#ifndef PHP_REQUEST_EXCEPTION_H
#define PHP_REQUEST_EXCEPTION_H


#include <stdexcept>


class RequestException: public std::runtime_error {

	public:

		using runtime_error::runtime_error;
};


#endif
