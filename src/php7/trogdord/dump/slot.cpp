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

// The constructor should NEVER be called in PHP userland. Instead, instances of
// Trogdord\Game\Dump\Slot will be returned by calls to \Trogdord\Game\Dump::getSlot().
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Slot, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
}

/*****************************************************************************/

// PHP Slot class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Slot, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_FE_END
};

/*****************************************************************************/

bool createDumpSlotObj(
	zval *slotObj,
	size_t slot,
	size_t timestampMs,
	zval *dumpObj
) {

	if (SUCCESS != object_init_ex(slotObj, SLOT_GLOBALS(classEntry))) {
		return false;
	}

	if (slot > ZEND_LONG_MAX) {
		zend_update_property_double(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_PROPERTY,
			strlen(SLOT_PROPERTY),
			slot
		);
	} else {
		zend_update_property_long(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_PROPERTY,
			strlen(SLOT_PROPERTY),
			slot
		);
	}

	if (timestampMs > ZEND_LONG_MAX) {
		zend_update_property_double(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_TIMESTAMP_PROPERTY,
			strlen(SLOT_TIMESTAMP_PROPERTY),
			timestampMs
		);
	} else {
		zend_update_property_long(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_TIMESTAMP_PROPERTY,
			strlen(SLOT_TIMESTAMP_PROPERTY),
			timestampMs
		);
	}

	zend_update_property(
		SLOT_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(slotObj),
		#else
			slotObj,
		#endif
		SLOT_DUMP_PROPERTY,
		strlen(SLOT_DUMP_PROPERTY),
		dumpObj
	);

	return true;
}

/*****************************************************************************/

void defineDumpSlotClass() {

	zend_class_entry dumpSlotClass;

	INIT_CLASS_ENTRY(dumpSlotClass, "Trogdord\\Game\\Dump\\Slot", classMethods);
	SLOT_GLOBALS(classEntry) = zend_register_internal_class(&dumpSlotClass);

	zend_declare_property_null(
		SLOT_GLOBALS(classEntry),
		SLOT_DUMP_PROPERTY,
		strlen(SLOT_DUMP_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	// Make sure users can't extend the class
	SLOT_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
