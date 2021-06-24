#include <trogdord/exception/requestexception.h>


RequestException::RequestException(const std::string &what_arg): ServerException(what_arg) {

	code = 500;
}

/***************************************************************************/

RequestException::RequestException(const char *what_arg): ServerException(what_arg) {

	code = 500;
}

/***************************************************************************/

RequestException::RequestException(const std::string &what_arg, unsigned int c):
ServerException(what_arg) {

	code = c;
}

/***************************************************************************/

RequestException::RequestException(const char *what_arg, unsigned int c):
ServerException(what_arg) {

	code = c;
}