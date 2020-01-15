#ifndef PHP_PLAYER_H
#define PHP_PLAYER_H

#include <trogdor/entities/player.h>
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

ZEND_EXTERN_MODULE_GLOBALS(being);

/*****************************************************************************/

// Declares the PHP Player class to the Zend engine.
extern void definePlayerClass();

#endif /* PHP_PLAYER_H */
