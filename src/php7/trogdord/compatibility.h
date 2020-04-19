#ifndef PHP_COMPATIBILITY_H
#define PHP_COMPATIBILITY_H

/* For compatibility with PHP 7.2 and below */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

// As of PHP 7.4, write_property handlers return a value
// See: https://github.com/php/php-src/blob/PHP-7.4/UPGRADING.INTERNALS
#if ZEND_MODULE_API_NO >= 20190902
	#define WRITE_PROP_RETURN_TYPE zval *
#else
	#define WRITE_PROP_RETURN_TYPE void
#endif

#endif /* PHP_COMPATIBILITY_H */
