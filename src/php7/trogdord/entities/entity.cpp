#include "../game.h"
#include "../trogdord.h"

#include "entity.h"

ZEND_DECLARE_MODULE_GLOBALS(entity);
ZEND_EXTERN_MODULE_GLOBALS(trogdord);
ZEND_EXTERN_MODULE_GLOBALS(game);

/*****************************************************************************/

// Entity Methods

// PHP Entity class methods
static const zend_function_entry entityMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/

void defineEntityClass() {

	zend_class_entry entityClass;

	INIT_CLASS_ENTRY(entityClass, "Trogdord\\Entity", entityMethods);
	ENTITY_GLOBALS(classEntry) = zend_register_internal_class(&entityClass);
	ENTITY_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	// Declare the Entity class's properties
	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"name",
		sizeof("name") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"title",
		sizeof("title") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"longDesc",
		sizeof("longDesc") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"shortDesc",
		sizeof("shortDesc") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);
}
