#ifndef CONFIG_UNDEFINED_VALUE_H
#define CONFIG_UNDEFINED_VALUE_H

#include "serverexception.h"


class ConfigUndefinedValue: public ServerException {

	public:

		using ServerException::ServerException;
};


#endif
