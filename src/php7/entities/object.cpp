#include "object.h"

ZEND_DECLARE_MODULE_GLOBALS(object);

/*****************************************************************************/
/*****************************************************************************/

// Object Methods

// PHP Object class methods
static const zend_function_entry objectMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

void defineObjectClass() {

	zend_class_entry objectClass;

	INIT_CLASS_ENTRY(objectClass, "Trogdor\\Entity\\Object", objectMethods);
	OBJECT_GLOBALS(classEntry) = zend_register_internal_class_ex(&objectClass, THING_GLOBALS(classEntry));
}
