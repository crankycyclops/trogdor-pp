#ifndef PHP_GAME_H
#define PHP_GAME_H

extern "C" {
	#include "php.h"
	#include "php_config.h"
}

#include "compatibility.h"

// Exception message when methods are called on a game that's already been destroyed
constexpr const char *GAME_ALREADY_DESTROYED = "Game has already been destroyed";

// The private property that stores the game's name
constexpr const char *GAME_NAME_PROPERTY = "name";

// The private property that stores the game's definition filename
constexpr const char *GAME_DEFINITION_PROPERTY = "definition";

// The private property that stores the game's id
constexpr const char *GAME_ID_PROPERTY = "id";

// The private property through which an instance of \Trogdord\Game can access
// the connection that spawned it
constexpr const char *TROGDORD_PROPERTY = "trogdord";

// True when the instance of \Trogdord\Game is valid and false if the game it
// refers to no longer exists server-side.
constexpr const char *GAME_VALID_PROPERTY = "valid";

/*****************************************************************************/

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
GAME_GLOBALS(classEntry), (THIS_PTR_COMPAT(THIS_PTR)), (PROPERTY), \
strlen((PROPERTY)), 1, (RV))

// Retrieve the instance of \Trogdord that spawned the instance of \Trogdord\Game.
#define GAME_TO_TROGDORD(THIS_PTR, RV) GAME_TO_PROP_VAL(THIS_PTR, RV, TROGDORD_PROPERTY)

// Retrieve the game id from an instance of \Trogdord\Game.
#define GAME_TO_ID(THIS_PTR, RV) GAME_TO_PROP_VAL(THIS_PTR, RV, GAME_ID_PROPERTY)

// Retrieve the "valid" property from an instance of \Trogdord\Game.
#define GAME_IS_VALID_PROP(THIS_PTR, RV) GAME_TO_PROP_VAL(THIS_PTR, RV, GAME_VALID_PROPERTY)

// Validate the instance of \Trogdord\Game before proceeding with an operation.
#define ASSERT_GAME_IS_VALID(ZVAL_VALID) \
if (IS_NULL == Z_TYPE_P(ZVAL_VALID) || IS_FALSE == Z_TYPE_P(ZVAL_VALID)) { \
	zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), GAME_ALREADY_DESTROYED, 0); \
	RETURN_NULL(); \
}

/*****************************************************************************/

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
