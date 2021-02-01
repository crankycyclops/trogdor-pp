#include "module.h"
#include "trogdord.h"
#include "game.h"
#include "phpexception.h"
#include "response.h"
#include "network/tcpconnectionmap.h"

#include "entities/entity.h"
#include "entities/tangible.h"
#include "entities/place.h"
#include "entities/thing.h"
#include "entities/being.h"
#include "entities/resource.h"
#include "entities/room.h"
#include "entities/object.h"
#include "entities/creature.h"
#include "entities/player.h"

#include "dump/dump.h"
#include "dump/slot.h"

PHP_MINFO_FUNCTION(trogdord) {

	php_info_print_table_start();
	php_info_print_table_row(2, "trogdord", "enabled");
	php_info_print_table_row(2, "Extension Version", PHP_TROGDORD_VERSION);
	php_info_print_table_end();
}

/*****************************************************************************/

PHP_MINIT_FUNCTION(trogdord) {

	defineTrogdordClass();
	defineGameClass();
	defineExceptionClasses();

	// Classes for managing dumped game state
	defineDumpClass();
	defineDumpSlotClass();

	// Entity class hierarchy
	defineEntityClass();
	defineTangibleClass();
	definePlaceClass();
	defineThingClass();
	defineBeingClass();
	defineResourceClass();
	defineRoomClass();
	defineObjectClass();
	defineCreatureClass();
	definePlayerClass();

	// This is the default port used to establish a connection to an instance
	// of trogdord when no port is specified in the call to \Trogdord's
	// constructor.
	REGISTER_LONG_CONSTANT(
		"Trogdord\\DEFAULT_PORT",
		TROGDORD_DEFAULT_PORT,
		CONST_CS | CONST_PERSISTENT
	);

	// Global namespaced constants that identify entity types and their string
	// representations. Note: I have to define these string constants here
	// instead of in a more appropriate location because the
	// REGISTER_STRING_CONSTANT macro relies on a variable which
	// PHP_MINIT_FUNCTION provides.
	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\ENTITY",
		(char *)ENTITY_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\TANGIBLE",
		(char *)TANGIBLE_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\PLACE",
		(char *)PLACE_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\THING",
		(char *)THING_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\BEING",
		(char *)BEING_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\RESOURCE",
		(char *)RESOURCE_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\ROOM",
		(char *)ROOM_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\OBJECT",
		(char *)OBJECT_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\CREATURE",
		(char *)CREATURE_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\PLAYER",
		(char *)PLAYER_TYPE_STR,
		CONST_CS | CONST_PERSISTENT
	);

	// Global namespaced constants that identify status codes returned as part
	// of a trogdord response.
	REGISTER_LONG_CONSTANT(
		"Trogdord\\Status\\SUCCESS",
		STATUS_SUCCESS,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_LONG_CONSTANT(
		"Trogdord\\Status\\PARTIAL_CONTENT",
		STATUS_PARTIAL_CONTENT,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_LONG_CONSTANT(
		"Trogdord\\Status\\INVALID",
		STATUS_INVALID,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_LONG_CONSTANT(
		"Trogdord\\Status\\NOT_FOUND",
		STATUS_NOT_FOUND,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_LONG_CONSTANT(
		"Trogdord\\Status\\CONFLICT",
		STATUS_CONFLICT,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_LONG_CONSTANT(
		"Trogdord\\Status\\INTERNAL_ERROR",
		STATUS_INTERNAL_ERROR,
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_LONG_CONSTANT(
		"Trogdord\\Status\\UNSUPPORTED",
		STATUS_UNSUPPORTED,
		CONST_CS | CONST_PERSISTENT
	);

	return SUCCESS;
}

/*****************************************************************************/

PHP_MSHUTDOWN_FUNCTION(trogdord) {

	return SUCCESS;
}

/*****************************************************************************/

zend_module_entry trogdord_module_entry = {

	STANDARD_MODULE_HEADER,
	MODULE_NAME,
	NULL,                   /* Functions */
	PHP_MINIT(trogdord),
	PHP_MSHUTDOWN(trogdord),
	NULL,                   /* RINIT */
	NULL,                   /* RSHUTDOWN */
	PHP_MINFO(trogdord),
	PHP_TROGDORD_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TROGDORD
extern "C" {
	ZEND_GET_MODULE(trogdord)
}
#endif
