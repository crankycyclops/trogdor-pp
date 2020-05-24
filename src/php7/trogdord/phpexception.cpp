#include "phpexception.h"

ZEND_DECLARE_MODULE_GLOBALS(exception);

/*****************************************************************************/

void defineExceptionClasses() {

	zend_class_entry ce;

	// Generic exception that all exceptions inherit from
	INIT_CLASS_ENTRY(ce, "Trogdord\\Exception", NULL);
	EXCEPTION_GLOBALS(baseException) = zend_register_internal_class_ex(&ce, zend_ce_exception);

	// Thrown if any kind of connection-related error occurs
	INIT_CLASS_ENTRY(ce, "Trogdord\\NetworkException", NULL);
	EXCEPTION_GLOBALS(networkException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));

	// Thrown any time a non-connection-related error occurs during a request
	INIT_CLASS_ENTRY(ce, "Trogdord\\RequestException", NULL);
	EXCEPTION_GLOBALS(requestException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));

	// Thrown any time an array representing a filter group or union is invalid
	INIT_CLASS_ENTRY(ce, "Trogdord\\FilterException", NULL);
	EXCEPTION_GLOBALS(filterException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));

	// Thrown if a request refers to a speciic game but that game doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\GameNotFound", NULL);
	EXCEPTION_GLOBALS(gameNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(requestException));

	// Thrown if a request refers to a specific entity in a game but that
	// entity doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\EntityNotFound", NULL);
	EXCEPTION_GLOBALS(entityNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(requestException));

	// Thrown if a request refers to a specific place (or any entity type that
	// inherits from Place) in a game but that place doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\PlaceNotFound", NULL);
	EXCEPTION_GLOBALS(placeNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(entityNotFound));

	// Thrown if a request refers to a specific thing (or any entity type that
	// inherits from Thing) in a game but that thing doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\ThingNotFound", NULL);
	EXCEPTION_GLOBALS(thingNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(entityNotFound));

	// Thrown if a request refers to a specific being (or any entity type that
	// inherits from Being) in a game but that being doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\BeingNotFound", NULL);
	EXCEPTION_GLOBALS(beingNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(thingNotFound));

	// Thrown if a request refers to a specific room but that room doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\RoomNotFound", NULL);
	EXCEPTION_GLOBALS(roomNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(placeNotFound));

	// Thrown if a request refers to a specific object but that object doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\ObjectNotFound", NULL);
	EXCEPTION_GLOBALS(objectNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(thingNotFound));

	// Thrown if a request refers to a specific creature but that creature
	// doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\CreatureNotFound", NULL);
	EXCEPTION_GLOBALS(creatureNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(beingNotFound));

	// Thrown if a request refers to a specific player but that player doesn't exist
	INIT_CLASS_ENTRY(ce, "Trogdord\\PlayerNotFound", NULL);
	EXCEPTION_GLOBALS(playerNotFound) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(beingNotFound));
}
