#ifndef PHP_RESOURCE_H
#define PHP_RESOURCE_H

#include "entity.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(resource)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(resource)

#ifdef ZTS
#define RESOURCE_GLOBALS(v) TSRMG(resource_globals_id, resource_globals *, v)
#else
#define RESOURCE_GLOBALS(v) (resource_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(entity);

/*****************************************************************************/

// Declares the PHP Resource class to the Zend engine.
extern void defineResourceClass();

#endif /* PHP_RESOURCE_H */
