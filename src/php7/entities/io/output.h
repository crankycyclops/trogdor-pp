#ifndef PHP_ENTITY_OUTPUT_H
#define PHP_ENTITY_OUTPUT_H

#include <trogdor/entities/entity.h>

extern "C" {
	#include "php.h"
}

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(entityout)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(entityout)

#ifdef ZTS
#define ENTITYOUT_GLOBALS(v) TSRMG(entityout_globals_id, entityout_globals *, v)
#else
#define ENTITYOUT_GLOBALS(v) (entityout_globals.v)
#endif

extern zend_object_handlers outputObjectHandlers;

/*****************************************************************************/

// Declares the PHP Trogdor\Entity\IO\Output class to the Zend engine.
extern void defineIOOutputClass();

#endif /* PHP_ENTITY_OUTPUT_H */
