#include "tangible.h"

ZEND_DECLARE_MODULE_GLOBALS(tangible);

/*****************************************************************************/

// Tangible Methods

// PHP Tangible class methods
static const zend_function_entry tangibleMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/

void defineTangibleClass() {

	zend_class_entry tangibleClass;

	INIT_CLASS_ENTRY(tangibleClass, "Trogdord\\Tangible", tangibleMethods);
	TANGIBLE_GLOBALS(classEntry) = zend_register_internal_class_ex(&tangibleClass, ENTITY_GLOBALS(classEntry));
	TANGIBLE_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
}
