#ifndef PHP_TANGIBLE_H
#define PHP_TANGIBLE_H

#include "entity.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(tangible)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(tangible)

#ifdef ZTS
#define TANGIBLE_GLOBALS(v) TSRMG(tangible_globals_id, tangible_globals *, v)
#else
#define TANGIBLE_GLOBALS(v) (tangible_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(entity);

/*****************************************************************************/

// Declares the PHP Tangible class to the Zend engine.
extern void defineTangibleClass();

#endif /* PHP_TANGIBLE_H */
