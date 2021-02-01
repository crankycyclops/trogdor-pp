#include "../trogdord.h"
#include "../json.h"
#include "../request.h"

#include "../phpexception.h"
#include "../exception/requestexception.h"

#include "dump.h"
#include "slot.h"

ZEND_DECLARE_MODULE_GLOBALS(slot);
ZEND_EXTERN_MODULE_GLOBALS(dump);

/*****************************************************************************/

// PHP Slot class methods
static const zend_function_entry classMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/

void defineDumpSlotClass() {

	zend_class_entry dumpSlotClass;

	INIT_CLASS_ENTRY(dumpSlotClass, "Trogdord\\Game\\Dump\\Slot", classMethods);
	SLOT_GLOBALS(classEntry) = zend_register_internal_class(&dumpSlotClass);

	zend_declare_property_null(
		SLOT_GLOBALS(classEntry),
		DUMP_PROPERTY,
		strlen(DUMP_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	// Make sure users can't extend the class
	SLOT_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
