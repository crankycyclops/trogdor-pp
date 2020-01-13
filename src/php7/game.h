#ifndef PHP_GAME_H
#define PHP_GAME_H

#include <trogdor/game.h>
#include <trogdor/parser/parsers/xmlparser.h>

extern "C" {
	#include "php.h"
}

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(game)
	zend_class_entry *gameClassEntry;
ZEND_END_MODULE_GLOBALS(game)

#ifdef ZTS
#define GAME_GLOBALS(v) TSRMG(game_globals_id, game_globals *, v)
#else
#define GAME_GLOBALS(v) (game_globals.v)
#endif

struct gameObject {
    trogdor::Game *realGameObject;
    zend_object std;
};

// For an explanation of why this is necessary, see:
// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
inline gameObject *ZOBJ_TO_GAMEOBJ(zend_object *zobj) {

    return (gameObject *)((char *)(zobj) - XtOffsetOf(gameObject, std));
}

extern zend_object_handlers gameObjectHandlers;

/*****************************************************************************/

// Declares the PHP Game class to the Zend engine.
extern void defineGameClass();

#endif /* PHP_GAME_H */
