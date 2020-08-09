#include "resource.h"

ZEND_DECLARE_MODULE_GLOBALS(resource);

/*****************************************************************************/

// Resource Methods

// PHP Resource class methods
static const zend_function_entry resourceMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/

void defineResourceClass() {

	zend_class_entry resourceClass;

	INIT_CLASS_ENTRY(resourceClass, "Trogdord\\Resource", resourceMethods);
	RESOURCE_GLOBALS(classEntry) = zend_register_internal_class_ex(&resourceClass, ENTITY_GLOBALS(classEntry));

	// Resource is a concrete entity type and can't be extended further
	RESOURCE_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
