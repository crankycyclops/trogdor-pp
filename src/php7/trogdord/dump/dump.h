#ifndef PHP_DUMP_H
#define PHP_DUMP_H

extern "C" {
	#include "php.h"
}

#include "compatibility.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(dump)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(dump)

#ifdef ZTS
#define DUMP_GLOBALS(v) TSRMG(dump_globals_id, dump_globals *, v)
#else
#define DUMP_GLOBALS(v) (dump_globals.v)
#endif

// The private property through which an instance of \Trogdord\Game\Dump can access
// the connection that spawned it.
constexpr const char *DUMP_TROGDORD_PROPERTY = "trogdord";

// Declares the \Trogdord\Game\Dump class to the Zend Engine.
extern void defineDumpClass();

#endif /* PHP_DUMP_H */
