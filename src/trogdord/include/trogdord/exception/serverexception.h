#ifndef SERVER_EXCEPTION_H
#define SERVER_EXCEPTION_H


#include <stdexcept>


class ServerException: public std::runtime_error {

	public:

		using runtime_error::runtime_error;
};


#endif
