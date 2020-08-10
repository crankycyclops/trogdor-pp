#ifndef PHP_THING_H
#define PHP_THING_H

#include "tangible.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(thing)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(thing)

#ifdef ZTS
#define THING_GLOBALS(v) TSRMG(thing_globals_id, thing_globals *, v)
#else
#define THING_GLOBALS(v) (thing_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(tangible);

/*****************************************************************************/

// Declares the PHP Thing class to the Zend engine.
extern void defineThingClass();

#endif /* PHP_THING_H */
