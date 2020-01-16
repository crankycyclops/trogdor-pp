#include "being.h"

ZEND_DECLARE_MODULE_GLOBALS(being);

/*****************************************************************************/
/*****************************************************************************/

// Being Methods

// PHP Being class methods
static const zend_function_entry beingMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

void defineBeingClass() {

	zend_class_entry beingClass;

	INIT_CLASS_ENTRY(beingClass, "Trogdor\\Entity\\Being", beingMethods);
	BEING_GLOBALS(classEntry) = zend_register_internal_class_ex(&beingClass, THING_GLOBALS(classEntry));
	BEING_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
}
