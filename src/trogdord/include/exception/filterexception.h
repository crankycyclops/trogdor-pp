#ifndef FILTER_EXCEPTION_H
#define FILTER_EXCEPTION_H

#include "serverexception.h"


class FilterException: public ServerException {

	public:

		using ServerException::ServerException;
};


#endif
