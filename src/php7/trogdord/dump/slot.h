#ifndef PHP_DUMP_SLOT_H
#define PHP_DUMP_SLOT_H

extern "C" {
	#include "php.h"
}

#include "compatibility.h"

// Property containing the slot's id
constexpr const char *SLOT_PROPERTY = "slot";

// Property containing the slot's timestamp in ms
constexpr const char *SLOT_TIMESTAMP_PROPERTY = "timestampMs";

// The private property through which an instance of \Trogdord\Game\Dump\Slot can access
// the dump object that contains it
constexpr const char *SLOT_DUMP_PROPERTY = "dump";

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
