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

// Declares Game class to the Zend Engine.
extern void defineGameClass();

#endif /* PHP_GAME_H */
