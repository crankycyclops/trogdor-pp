#ifndef PHP_DUMP_SLOT_H
#define PHP_DUMP_SLOT_H

extern "C" {
	#include "php.h"
}

#include "compatibility.h"

// Exception message when methods are called on a dump slot that's already been destroyed
constexpr const char *SLOT_ALREADY_DESTROYED = "Dump slot has already been destroyed";

// Property containing the slot's id
constexpr const char *SLOT_SLOT_PROPERTY = "slot";

// Property containing the slot's timestamp in ms
constexpr const char *SLOT_TIMESTAMP_PROPERTY = "timestampMs";

// The private property through which an instance of \Trogdord\Game\Dump\Slot can access
// the dump object that contains it
constexpr const char *SLOT_DUMP_PROPERTY = "dump";

// Whether or not the instance of \Trogdord\Game\Dump\Slot is valid
constexpr const char *SLOT_VALID_PROPERTY = "valid";

/*****************************************************************************/

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(slot)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(slot)

#ifdef ZTS
#define SLOT_GLOBALS(v) TSRMG(slot_globals_id, slot_globals *, v)
#else
#define SLOT_GLOBALS(v) (slot_globals.v)
#endif

// Retrieve the specified property for an instance of \Trogdord\Game\Dump\Slot.
// Return type is zval *.
#define SLOT_TO_PROP_VAL(THIS_PTR, RV, PROPERTY) zend_read_property(\
SLOT_GLOBALS(classEntry), (THIS_PTR_COMPAT(THIS_PTR)), (PROPERTY), \
strlen((PROPERTY)), 1, (RV))

// Retrieve the slot number.
#define SLOT_TO_SLOTVAL(THIS_PTR, RV) SLOT_TO_PROP_VAL(THIS_PTR, RV, SLOT_SLOT_PROPERTY)

// Retrieve the dump object the slot belongs to.
#define SLOT_TO_DUMP(THIS_PTR, RV) SLOT_TO_PROP_VAL(THIS_PTR, RV, SLOT_DUMP_PROPERTY)

// Retrieve the "valid" property from an instance of \Trogdord\Game\Dump\Slot.
#define SLOT_IS_VALID_PROP(THIS_PTR, RV) SLOT_TO_PROP_VAL(THIS_PTR, RV, SLOT_VALID_PROPERTY)

// Validate the instance of \Trogdord\Game\Dump\Slot before proceeding with an operation.
#define ASSERT_DUMP_SLOT_IS_VALID(ZVAL_VALID) \
if (IS_NULL == Z_TYPE_P(ZVAL_VALID) || IS_FALSE == Z_TYPE_P(ZVAL_VALID)) { \
	zend_throw_exception(EXCEPTION_GLOBALS(dumpSlotNotFound), SLOT_ALREADY_DESTROYED, 0); \
	RETURN_NULL(); \
}

// Invalidate the slot object
#if ZEND_MODULE_API_NO >= 20200930
	#define INVALIDATE_SLOT(ZVAL) \
	zend_update_property_bool( \
		SLOT_GLOBALS(classEntry), \
		Z_OBJ_P(ZVAL), \
		SLOT_VALID_PROPERTY, \
		strlen(SLOT_VALID_PROPERTY), \
		0 \
	)
#else
	#define INVALIDATE_SLOT(ZVAL) \
	zend_update_property_bool( \
		SLOT_GLOBALS(classEntry), \
		ZVAL, \
		SLOT_VALID_PROPERTY, \
		strlen(SLOT_VALID_PROPERTY), \
		0 \
	)
#endif

/*****************************************************************************/

// Creates an instance of \Trogdord\Game\Dump\Slot. Returns true on success and
// false on error.
extern bool createDumpSlotObj(
	zval *slotObj,
	size_t slot,
	size_t timestampMs,
	zval *dumpObj
);

// Declares the \Trogdord\Game\Dump\Slot class to the Zend Engine.
extern void defineDumpSlotClass();

#endif /* PHP_DUMP_SLOT_H */
