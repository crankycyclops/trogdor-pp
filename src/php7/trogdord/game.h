#ifndef PHP_GAME_H
#define PHP_GAME_H

extern "C" {
	#include "php.h"
}

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(game)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(game)

#ifdef ZTS
#define GAME_GLOBALS(v) TSRMG(game_globals_id, game_globals *, v)
#else
#define GAME_GLOBALS(v) (game_globals.v)
#endif

/*****************************************************************************/

// The private property that stores the game's id
extern const char *GAME_ID_PROPERTY_NAME;

// The private property through which an instance of \Trogdord\Game can access
// the connection that spawned it
extern const char *TROGDORD_PROPERTY_NAME;

// Creates an instance of \Trogdord\Game. Returns true on success and false on
// error.
extern bool createGameObj(zval *gameObj, size_t id, zval *trogdordObj);

// Declares Game class to the Zend Engine.
extern void defineGameClass();

/*****************************************************************************/

// Retrieve the instance of \Trogdord that spawned the instance of \Trogdord\Game.
#define GAME_TO_TROGDORD(thisPtr, rv) zend_read_property(\
GAME_GLOBALS(classEntry), thisPtr, TROGDORD_PROPERTY_NAME, \
strlen(TROGDORD_PROPERTY_NAME), 1, rv TSRMLS_CC)

// Retrieve the game id from an instance of \Trogdord\Game.
#define GAME_TO_ID(thisPtr, rv) zend_read_property(\
GAME_GLOBALS(classEntry), thisPtr, GAME_ID_PROPERTY_NAME, \
strlen(GAME_ID_PROPERTY_NAME), 1, rv TSRMLS_CC)

// Validate the instance of \Trogdord\Game before proceeding with an operation.
#define ASSERT_GAME_ID_IS_VALID(ZVAL_ID) \
if (IS_NULL == ZVAL_ID) { \
	zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), GAME_ALREADY_DESTROYED, 0); \
	RETURN_NULL(); \
}

#endif /* PHP_GAME_H */
