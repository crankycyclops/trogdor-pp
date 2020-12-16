#ifndef SERIAL_DRIVER_NOT_FOUND_EXCEPTION_H
#define SERIAL_DRIVER_NOT_FOUND_EXCEPTION_H


#include "serverexception.h"


class SerialDriverNotFound: public ServerException {

	public:

		using ServerException::ServerException;

		// Don't require a message for this one, because the meaning of the
		// exception is obvious without it.
		inline SerialDriverNotFound(): ServerException("") {}
};


#endif
