#ifndef PHP_CREATURE_H
#define PHP_CREATURE_H

#include "being.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(creature)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(creature)

#ifdef ZTS
#define CREATURE_GLOBALS(v) TSRMG(creature_globals_id, creature_globals *, v)
#else
#define CREATURE_GLOBALS(v) (creature_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(being);

/*****************************************************************************/

// Declares the PHP Creature class to the Zend engine.
extern void defineCreatureClass();

#endif /* PHP_CREATURE_H */
