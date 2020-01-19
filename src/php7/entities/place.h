#ifndef PHP_PLACE_H
#define PHP_PLACE_H

#include <trogdor/entities/place.h>
#include "entity.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(place)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(place)

#ifdef ZTS
#define PLACE_GLOBALS(v) TSRMG(place_globals_id, place_globals *, v)
#else
#define PLACE_GLOBALS(v) (place_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(entity);

/*****************************************************************************/

// Utility function that refreshes the properties of an instance of
// Trogdor\Entity\Place based on their current underlying values.
extern void refreshPlaceProperties(trogdor::entity::Entity *ePtr, zval *phpEntityObj);

// Declares the PHP Place class to the Zend engine.
extern void definePlaceClass();

#endif /* PHP_PLACE_H */
