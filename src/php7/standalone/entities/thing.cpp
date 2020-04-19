#include "thing.h"

ZEND_DECLARE_MODULE_GLOBALS(thing);

/*****************************************************************************/
/*****************************************************************************/

// Thing Methods

// PHP Thing class methods
static const zend_function_entry thingMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

// Utility functions

void refreshThingProperties(trogdor::entity::Entity *ePtr, zval *phpEntityObj) {

	refreshEntityProperties(ePtr, phpEntityObj);
}

/*****************************************************************************/

void defineThingClass() {

	zend_class_entry thingClass;

	INIT_CLASS_ENTRY(thingClass, "Trogdor\\Entity\\Thing", thingMethods);
	THING_GLOBALS(classEntry) = zend_register_internal_class_ex(&thingClass, ENTITY_GLOBALS(classEntry));
	THING_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
}