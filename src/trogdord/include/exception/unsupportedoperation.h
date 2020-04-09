#ifndef UNSUPPORTED_OPERATION_H
#define UNSUPPORTED_OPERATION_H

#include "serverexception.h"


class UnsupportedOperation: public ServerException {

	public:

		using ServerException::ServerException;
};


#endif
