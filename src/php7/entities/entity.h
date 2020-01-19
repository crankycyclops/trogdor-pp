#ifndef PHP_ENTITY_H
#define PHP_ENTITY_H

#include <trogdor/entities/entity.h>

extern "C" {
	#include "php.h"
}

#include "entities/io/input.h"
#include "entities/io/output.h"

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

// Custom data that will be instantiated alongside the zend_object
struct customEntityData {
	bool managedByGame;
	trogdor::entity::Entity *obj;
};

struct entityObject {
	customEntityData realEntityObject;
	zend_object std;
};

// For an explanation of why this is necessary, see:
// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
inline entityObject *ZOBJ_TO_ENTITYOBJ(zend_object *zobj) {

	return (entityObject *)((char *)(zobj) - XtOffsetOf(entityObject, std));
}

extern zend_object_handlers entityObjectHandlers;

/*****************************************************************************/

ZEND_EXTERN_MODULE_GLOBALS(entityin);
ZEND_EXTERN_MODULE_GLOBALS(entityout);

// Utility function that attaches an instance of Trogdor\Entity\IO\Input to a
// PHP Entity class.
void attachInputToEntity(trogdor::entity::Entity *ePtr, zval *phpEntityObj);

// Utility function that attaches an instance of Trogdor\Entity\IO\Output to a
// PHP Entity class.
extern void attachOutputToEntity(trogdor::entity::Entity *ePtr, zval *phpEntityObj);

// Utility function that refreshes the properties of an instance of
// Trogdor\Entity\Entity based on their current underlying values.
extern void refreshEntityProperties(trogdor::entity::Entity *ePtr, zval *phpEntityObj);

// Overrides zend_std_write_property and makes certain properties read-only
extern void writeProperty(zval *object, zval *member, zval *value, void **cache_slot);

// Declares the PHP Entity class to the Zend engine.
extern void defineEntityClass();

#endif /* PHP_ENTITY_H */
