#ifndef CONFIG_INVALID_VALUE_H
#define CONFIG_INVALID_VALUE_H

#include "serverexception.h"


class ConfigInvalidValue: public ServerException {

	public:

		using ServerException::ServerException;
};


#endif
