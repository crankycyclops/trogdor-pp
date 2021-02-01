#ifndef PHP_DUMP_SLOT_H
#define PHP_DUMP_SLOT_H

extern "C" {
	#include "php.h"
}

#include "compatibility.h"

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

// The private property through which an instance of \Trogdord\Game\Dump\Slot can access
// the dump object that contains it
constexpr const char *DUMP_PROPERTY = "dump";

// Declares the \Trogdord\Game\Dump\Slot class to the Zend Engine.
extern void defineDumpSlotClass();

#endif /* PHP_DUMP_SLOT_H */
