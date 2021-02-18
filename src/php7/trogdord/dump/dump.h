#ifndef PHP_DUMP_H
#define PHP_DUMP_H

extern "C" {
	#include "php.h"
	#include "php_config.h"
}

#include "compatibility.h"

// Exception message when methods are called on a game dump that's already been destroyed
constexpr const char *DUMP_ALREADY_DESTROYED = "Dump has already been destroyed";

// Dumped game's id
constexpr const char *DUMP_ID_PROPERTY = "id";

// Dumped game's name
constexpr const char *DUMP_NAME_PROPERTY = "name";

// Definition file that was used to create the dumped game
constexpr const char *DUMP_DEFINITION_PROPERTY = "definition";

// The private property through which an instance of \Trogdord\Game\Dump can access
// the connection that spawned it.
constexpr const char *DUMP_TROGDORD_PROPERTY = "trogdord";

// Whether or not the instance of \Trogdord\Game\Dump is valid
constexpr const char *DUMP_VALID_PROPERTY = "valid";

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

// Retrieve the game id.
#define DUMP_TO_ID(THIS_PTR, RV) DUMP_TO_PROP_VAL(THIS_PTR, RV, DUMP_ID_PROPERTY)

// Retrieve the "valid" property from an instance of \Trogdord\Game\Dump.
#define DUMP_IS_VALID_PROP(THIS_PTR, RV) DUMP_TO_PROP_VAL(THIS_PTR, RV, DUMP_VALID_PROPERTY)

// Validate the instance of \Trogdord\Game\Dump before proceeding with an operation.
#define ASSERT_DUMP_IS_VALID(ZVAL_VALID) \
if (IS_NULL == Z_TYPE_P(ZVAL_VALID) || IS_FALSE == Z_TYPE_P(ZVAL_VALID)) { \
	zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), DUMP_ALREADY_DESTROYED, 0); \
	RETURN_NULL(); \
}

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
