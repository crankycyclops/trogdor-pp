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

// The private property which contains the entity's name
extern const char *ENTITY_PROPERTY_NAME;

// Maps entity type names to their zend class entries
extern const std::unordered_map<std::string, zend_class_entry *> EntityTypes;

/*****************************************************************************/

// Creates a PHP instance corresponding to the given trogdord entity. Returns
// true on success and false on error.
extern bool createEntityObj(zval *entityObj, JSONObject properties, zval *gameObj);

// Declares the PHP Entity class to the Zend engine.
extern void defineEntityClass();

#endif /* PHP_ENTITY_H */
