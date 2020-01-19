#ifndef PHP_OBJECT_H
#define PHP_OBJECT_H

#include <trogdor/entities/object.h>
#include "thing.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(object)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(object)

#ifdef ZTS
#define OBJECT_GLOBALS(v) TSRMG(object_globals_id, object_globals *, v)
#else
#define OBJECT_GLOBALS(v) (object_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(thing);

/*****************************************************************************/

// Utility function that refreshes the properties of an instance of
// Trogdor\Entity\Object based on their current underlying values.
extern void refreshObjectProperties(trogdor::entity::Entity *ePtr, zval *phpEntityObj);

// Declares the PHP Object class to the Zend engine.
extern void defineObjectClass();

#endif /* PHP_OBJECT_H */
