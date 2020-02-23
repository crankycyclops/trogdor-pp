#ifndef GAME_NOT_FOUND_H
#define GAME_NOT_FOUND_H

#include "serverexception.h"


class GameNotFound: public ServerException {

	public:

		using ServerException::ServerException;

		// Don't require a message for this one, because the meaning of the
		// exception is obvious without it.
		inline GameNotFound(): ServerException("") {}
};


#endif
