#ifndef PHP_EXCEPTION_H
#define PHP_EXCEPTION_H

#include <trogdor/exception/exception.h>
#include <trogdor/exception/validationexception.h>
#include <trogdor/exception/undefinedexception.h>
#include <trogdor/exception/parseexception.h>
#include <trogdor/exception/luaexception.h>
#include <trogdor/exception/entityexception.h>
#include <trogdor/exception/beingexception.h>

extern "C" {
	#include "php.h"
	#include "zend_exceptions.h"
}

ZEND_BEGIN_MODULE_GLOBALS(exception)
	zend_class_entry *baseException;
	zend_class_entry *validationException;
	zend_class_entry *undefinedException;
	zend_class_entry *parseException;
	zend_class_entry *luaException;
	zend_class_entry *entityException;
	zend_class_entry *beingException;
ZEND_END_MODULE_GLOBALS(exception)

#ifdef ZTS
#define EXCEPTION_GLOBALS(v) TSRMG(exception_globals_id, exception_globals *, v)
#else
#define EXCEPTION_GLOBALS(v) (exception_globals.v)
#endif

// Make sure globals are externally accessible
ZEND_EXTERN_MODULE_GLOBALS(exception)

/*****************************************************************************/

// Declares Trogdor Exception classes to the Zend Engine.
extern void defineExceptionClasses();

#endif /* PHP_EXCEPTION_H */
