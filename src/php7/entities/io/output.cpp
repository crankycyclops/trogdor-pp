#include "output.h"

ZEND_DECLARE_MODULE_GLOBALS(entityout);

zend_object_handlers outputObjectHandlers;

/*****************************************************************************/
/*****************************************************************************/

// Output Methods

// Trogdor\Entity\IO\Output methods
static const zend_function_entry outputMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

void defineIOOutputClass() {

	zend_class_entry outputClass;

	INIT_CLASS_ENTRY(outputClass, "Trogdor\\Entity\\IO\\Output", outputMethods);
	ENTITYOUT_GLOBALS(classEntry) = zend_register_internal_class(&outputClass);
	ENTITYOUT_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
