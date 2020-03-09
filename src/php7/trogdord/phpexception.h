#ifndef PHP_EXCEPTION_H
#define PHP_EXCEPTION_H

extern "C" {
	#include "php.h"
	#include "zend_exceptions.h"
}

ZEND_BEGIN_MODULE_GLOBALS(exception)
	zend_class_entry *baseException;
	zend_class_entry *networkException;
	zend_class_entry *requestException;
	zend_class_entry *gameNotFound;
	zend_class_entry *entityNotFound;
	zend_class_entry *placeNotFound;
	zend_class_entry *thingNotFound;
	zend_class_entry *beingNotFound;
	zend_class_entry *roomNotFound;
	zend_class_entry *objectNotFound;
	zend_class_entry *creatureNotFound;
	zend_class_entry *playerNotFound;
ZEND_END_MODULE_GLOBALS(exception)

#ifdef ZTS
	#define EXCEPTION_GLOBALS(v) TSRMG(exception_globals_id, exception_globals *, v)
#else
	#define EXCEPTION_GLOBALS(v) (exception_globals.v)
#endif

// Make sure globals are externally accessible
ZEND_EXTERN_MODULE_GLOBALS(exception)

/*****************************************************************************/

// Declares Trogdord Exception classes to the Zend Engine.
extern void defineExceptionClasses();

#endif /* PHP_EXCEPTION_H */
