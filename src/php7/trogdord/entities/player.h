#ifndef PHP_PLAYER_H
#define PHP_PLAYER_H

#include "being.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(player)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(player)

#ifdef ZTS
#define PLAYER_GLOBALS(v) TSRMG(player_globals_id, player_globals *, v)
#else
#define PLAYER_GLOBALS(v) (player_globals.v)
#endif

// Validate the instance of \Trogdord\Player before proceeding with an operation.
#define ASSERT_PLAYER_NAME_IS_VALID(ZVAL_NAME) \
if (IS_NULL == ZVAL_NAME) { \
	zend_throw_exception(EXCEPTION_GLOBALS(playerNotFound), PLAYER_ALREADY_DESTROYED, 0); \
	RETURN_NULL(); \
}

ZEND_EXTERN_MODULE_GLOBALS(being);

/*****************************************************************************/

// Exception message when methods are called on a player that's already been destroyed
constexpr const char *PLAYER_ALREADY_DESTROYED = "Player has already been destroyed";

// Declares the PHP Player class to the Zend engine.
extern void definePlayerClass();

#endif /* PHP_PLAYER_H */
