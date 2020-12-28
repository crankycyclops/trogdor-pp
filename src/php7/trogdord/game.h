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

// Retrieve the specified property for an instance of \Trogdord\Game. Return
// type is zval *.
#define GAME_TO_PROP_VAL(THIS_PTR, RV, PROPERTY) zend_read_property(\
GAME_GLOBALS(classEntry), (THIS_PTR), (PROPERTY), \
strlen((PROPERTY)), 1, (RV))

// Retrieve the instance of \Trogdord that spawned the instance of \Trogdord\Game.
#define GAME_TO_TROGDORD(THIS_PTR, RV) GAME_TO_PROP_VAL(THIS_PTR, RV, TROGDORD_PROPERTY)

// Retrieve the game id from an instance of \Trogdord\Game.
#define GAME_TO_ID(THIS_PTR, RV) GAME_TO_PROP_VAL(THIS_PTR, RV, GAME_ID_PROPERTY)

// Validate the instance of \Trogdord\Game before proceeding with an operation.
#define ASSERT_GAME_ID_IS_VALID(ZVAL_ID) \
if (IS_NULL == ZVAL_ID) { \
	zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), GAME_ALREADY_DESTROYED, 0); \
	RETURN_NULL(); \
}

/*****************************************************************************/

// Exception message when methods are called on a game that's already been destroyed
extern const char *GAME_ALREADY_DESTROYED;

// The private property that stores the game's name
extern const char *GAME_NAME_PROPERTY;

// The private property that stores the game's definition filename
extern const char *GAME_DEFINITION_PROPERTY;

// The private property that stores the game's id
extern const char *GAME_ID_PROPERTY;

// The private property through which an instance of \Trogdord\Game can access
// the connection that spawned it
extern const char *TROGDORD_PROPERTY;

// Creates an instance of \Trogdord\Game. Returns true on success and false on
// error.
extern bool createGameObj(
	zval *gameObj,
	std::string name,
	std::string definition,
	size_t id,
	zval *trogdordObj
);

// Declares Game class to the Zend Engine.
extern void defineGameClass();

#endif /* PHP_GAME_H */
