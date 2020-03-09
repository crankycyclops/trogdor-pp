#ifndef PHP_NETWORK_EXCEPTION_H
#define PHP_NETWORK_EXCEPTION_H


#include <stdexcept>


class NetworkException: public std::runtime_error {

	public:

		using runtime_error::runtime_error;
};


#endif
