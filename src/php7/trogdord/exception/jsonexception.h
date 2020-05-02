#ifndef PHP_JSON_EXCEPTION_H
#define PHP_JSON_EXCEPTION_H


#include <stdexcept>


class JSONException: public std::runtime_error {

	public:

	using runtime_error::runtime_error;
};


#endif
