#include "requestexception.h"


RequestException::RequestException(const std::string &what_arg):
	std::runtime_error(what_arg), code(DEFAULT_CODE) {}

/***************************************************************************/

RequestException::RequestException(const char *what_arg):
	std::runtime_error(what_arg), code(DEFAULT_CODE) {}

/***************************************************************************/

RequestException::RequestException(const std::string &what_arg, unsigned codeArg):
	std::runtime_error(what_arg), code(codeArg) {}

/***************************************************************************/

RequestException::RequestException(const char *what_arg, unsigned codeArg):
	std::runtime_error(what_arg), code(codeArg) {}
