#ifndef PHP_TROGDORD_H
#define PHP_TROGDORD_H

// If a port isn't specified when the Trogdord constructor is invoked, we assume
// that the daemon is running on this default port.
#define TROGDORD_DEFAULT_PORT 1040

extern "C" {
	#include "php.h"
}

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(trogdord)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(trogdord)

#ifdef ZTS
#define TROGDORD_GLOBALS(v) TSRMG(trogdord_globals_id, trogdord_globals *, v)
#else
#define TROGDORD_GLOBALS(v) (trogdord_globals.v)
#endif

/*****************************************************************************/

// Declares Trogdord class to the Zend Engine.
extern void defineTrogdordClass();

#endif /* PHP_TROGDORD_H */
