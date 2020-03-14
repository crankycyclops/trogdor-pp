#ifndef PHP_BEING_H
#define PHP_BEING_H

#include "thing.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(being)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(being)

#ifdef ZTS
#define BEING_GLOBALS(v) TSRMG(being_globals_id, being_globals *, v)
#else
#define BEING_GLOBALS(v) (being_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(thing);

/*****************************************************************************/

// Declares the PHP Being class to the Zend engine.
extern void defineBeingClass();

#endif /* PHP_BEING_H */
