#ifndef PHP_ENTITY_H
#define PHP_ENTITY_H

extern "C" {
	#include "php.h"
}

#include "../json.h"

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

// Retrieve the specified property for an instance of \Trogdord\Entity. Return
// type is zval *.
#define ENTITY_TO_PROP_VAL(THIS_PTR, RV, PROPERTY) zend_read_property(\
ENTITY_GLOBALS(classEntry), (THIS_PTR), (PROPERTY), \
strlen((PROPERTY)), 1, (RV) TSRMLS_CC)

// The private property which contains the entity's name
extern const char *ENTITY_PROPERTY_NAME;

// String representations of each entity type
extern const char *ENTITY_TYPE_STR;
extern const char *PLACE_TYPE_STR;
extern const char *THING_TYPE_STR;
extern const char *BEING_TYPE_STR;
extern const char *ROOM_TYPE_STR;
extern const char *OBJECT_TYPE_STR;
extern const char *CREATURE_TYPE_STR;
extern const char *PLAYER_TYPE_STR;

/*****************************************************************************/

// Creates a PHP instance corresponding to the given trogdord entity. Returns
// true on success and false on error.
extern bool createEntityObj(zval *entityObj, JSONObject properties, zval *gameObj);

// Declares the PHP Entity class to the Zend engine.
extern void defineEntityClass();

#endif /* PHP_ENTITY_H */
