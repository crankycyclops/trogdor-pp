#ifndef PHP_ROOM_H
#define PHP_ROOM_H

#include <trogdor/entities/room.h>
#include "place.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(room)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(room)

#ifdef ZTS
#define ROOM_GLOBALS(v) TSRMG(room_globals_id, room_globals *, v)
#else
#define ROOM_GLOBALS(v) (room_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(place);

/*****************************************************************************/

// Declares the PHP Room class to the Zend engine.
extern void defineRoomClass();

#endif /* PHP_ROOM_H */
