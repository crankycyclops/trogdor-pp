#ifndef ENTITY_NOT_FOUND_H
#define ENTITY_NOT_FOUND_H

#include "../serverexception.h"


class EntityNotFound: public ServerException {

	public:

		using ServerException::ServerException;

		// Don't require a message for this one, because the meaning of the
		// exception is obvious without it.
		inline EntityNotFound(): ServerException("") {}
};


#endif
