#ifndef PHP_DUMP_H
#define PHP_DUMP_H

extern "C" {
	#include "php.h"
}

#include "compatibility.h"

// Dumped game's name
constexpr const char *DUMP_NAME_PROPERTY = "name";

// Definition file that was used to create the dumped game
constexpr const char *DUMP_DEFINITION_PROPERTY = "definition";

// Dumped game's id
constexpr const char *DUMP_ID_PROPERTY = "id";

// The private property through which an instance of \Trogdord\Game\Dump can access
// the connection that spawned it.
constexpr const char *DUMP_TROGDORD_PROPERTY = "trogdord";

/*****************************************************************************/

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

// Retrieve the specified property for an instance of \Trogdord\Game\Dump. Return
// type is zval *.
#define DUMP_TO_PROP_VAL(THIS_PTR, RV, PROPERTY) zend_read_property(\
DUMP_GLOBALS(classEntry), (THIS_PTR_COMPAT(THIS_PTR)), (PROPERTY), \
strlen((PROPERTY)), 1, (RV))

// Retrieve the underlying instance of \Trogdord.
#define DUMP_TO_TROGDORD(THIS_PTR, RV) DUMP_TO_PROP_VAL(THIS_PTR, RV, DUMP_TROGDORD_PROPERTY)

/*****************************************************************************/

// Creates an instance of \Trogdord\Game\Dump. Returns true on success and false on
// error.
extern bool createDumpObj(
	zval *dumpObj,
	size_t id,
	std::string name,
	std::string definition,
	size_t created,
	zval *trogdordObj
);

// Declares the \Trogdord\Game\Dump class to the Zend Engine.
extern void defineDumpClass();

#endif /* PHP_DUMP_H */
