#ifndef PHP_REQUEST_EXCEPTION_H
#define PHP_REQUEST_EXCEPTION_H


#include <stdexcept>


class RequestException: public std::runtime_error {

	protected:

		// Response status code
		unsigned code;

	public:

		// If code isn't specified when the exception is thrown, we assume this
		static const unsigned DEFAULT_CODE = 500;

		// Contructors
		explicit RequestException(const std::string &what_arg);
		explicit RequestException(const char *what_arg);
		explicit RequestException(const std::string &what_arg, unsigned code);
		explicit RequestException(const char *what_arg, unsigned code);

		// Returns the response status code
		inline const unsigned getCode() const noexcept {return code;}
};


#endif
