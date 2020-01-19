#ifndef PHP_GAME_H
#define PHP_GAME_H

#include <vector>

#include <trogdor/game.h>
#include <trogdor/parser/parsers/xmlparser.h>

extern "C" {
	#include "php.h"
}

#include "gamedata.h"

#include "entities/room.h"
#include "entities/object.h"
#include "entities/creature.h"
#include "entities/player.h"

#include "iostream/streamin.h"
#include "iostream/streamout.h"
#include "iostream/streamerr.h"

#include "entities/io/input.h"
#include "entities/io/output.h"

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

struct gameObject {
	customData realGameObject;
	zend_object std;
};

// For an explanation of why this is necessary, see:
// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
inline gameObject *ZOBJ_TO_GAMEOBJ(zend_object *zobj) {

	return (gameObject *)((char *)(zobj) - XtOffsetOf(gameObject, std));
}

extern zend_object_handlers gameObjectHandlers;

/*****************************************************************************/

ZEND_EXTERN_MODULE_GLOBALS(room);
ZEND_EXTERN_MODULE_GLOBALS(object);
ZEND_EXTERN_MODULE_GLOBALS(creature);
ZEND_EXTERN_MODULE_GLOBALS(player);

ZEND_EXTERN_MODULE_GLOBALS(entityin);
ZEND_EXTERN_MODULE_GLOBALS(entityout);

// Retrive a persisted game by index
extern trogdor::Game *getGameById(size_t id);

// Persist a game and return its id. This function does not check if a Game *
// has already been persisted. Instead, that check should occur in the PHP
// "persist" method.
extern size_t persistGame(trogdor::Game *game);

// Depersist a game referenced by its id. Only call this from the PHP depersist
// method! If you depersist a Game object after its corresponding PHP class has
// been destroyed, you'll have a memory leak.
extern void depersistGame(size_t id);

// Call this during MSHUTDOWN to delete all remaining Game pointers.
extern void reapPersistedGames();

// Declares the PHP Game class to the Zend engine.
extern void defineGameClass();

#endif /* PHP_GAME_H */
