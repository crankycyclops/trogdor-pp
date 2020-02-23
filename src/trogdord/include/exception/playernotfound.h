#ifndef PLAYER_NOT_FOUND_H
#define PLAYER_NOT_FOUND_H

#include "serverexception.h"


class PlayerNotFound: public ServerException {

	public:

		using ServerException::ServerException;

		// Don't require a message for this one, because the meaning of the
		// exception is obvious without it.
		inline PlayerNotFound(): ServerException("") {}
};


#endif
