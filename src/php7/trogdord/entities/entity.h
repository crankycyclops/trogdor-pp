#ifndef PHP_ENTITY_H
#define PHP_ENTITY_H

extern "C" {
	#include "php.h"
}

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(entity)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(entity)

#ifdef ZTS
#define ENTITY_GLOBALS(v) TSRMG(entity_globals_id, entity_globals *, v)
#else
#define ENTITY_GLOBALS(v) (entity_globals.v)
#endif

/*****************************************************************************/

// Declares the PHP Entity class to the Zend engine.
extern void defineEntityClass();

#endif /* PHP_ENTITY_H */
