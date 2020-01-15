#include "room.h"

ZEND_DECLARE_MODULE_GLOBALS(room);

/*****************************************************************************/
/*****************************************************************************/

// Room Methods

// PHP Room class methods
static const zend_function_entry roomMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

void defineRoomClass() {

	zend_class_entry roomClass;

	INIT_CLASS_ENTRY(roomClass, "Trogdor\\Entity\\Room", roomMethods);
	ROOM_GLOBALS(classEntry) = zend_register_internal_class_ex(&roomClass, PLACE_GLOBALS(classEntry));
}
