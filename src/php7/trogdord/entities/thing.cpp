#include "thing.h"

ZEND_DECLARE_MODULE_GLOBALS(thing);

/*****************************************************************************/

// Thing Methods

// PHP Thing class methods
static const zend_function_entry thingMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/

void defineThingClass() {

	zend_class_entry thingClass;

	INIT_CLASS_ENTRY(thingClass, "Trogdord\\Thing", thingMethods);
	THING_GLOBALS(classEntry) = zend_register_internal_class_ex(&thingClass, TANGIBLE_GLOBALS(classEntry));
	THING_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
}
