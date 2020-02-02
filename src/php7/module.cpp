#include "module.h"
#include "game.h"
#include "exception.h"

#include "entities/room.h"
#include "entities/object.h"
#include "entities/creature.h"
#include "entities/player.h"

PHP_MINFO_FUNCTION(trogdor) {

	php_info_print_table_start();
	php_info_print_table_row(2, "Trogdor++ Support", "enabled");
	php_info_print_table_row(2, "Extension Version", PHP_TROGDOR_VERSION);
	php_info_print_table_row(2, "Built on", __DATE__ " at " __TIME__);

	#ifdef ZTS
		php_info_print_table_row(2, "ZTS", "yes");
	#else
		php_info_print_table_row(2, "ZTS", "no");
	#endif

	php_info_print_table_row(2, "Persistent Games", std::to_string(getNumPersistedGames()).c_str());
	php_info_print_table_end();
}

/*****************************************************************************/

PHP_MINIT_FUNCTION(trogdor) {

	// Entity class hierarchy
	defineEntityClass();
	definePlaceClass();
	defineThingClass();
	defineRoomClass();
	defineObjectClass();
	defineBeingClass();
	defineCreatureClass();
	definePlayerClass();

	defineGameClass();
	defineExceptionClasses();

	// Global namespaced constants that identify entity types and their string
	// representations. Note: I have to define these string constants here
	// instead of in a more appropriate location because the
	// REGISTER_STRING_CONSTANT macro relies on a variable, module_number,
	// which PHP_MINIT_FUNCTION provides.
	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_ENTITY",
		(char *)"entity",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_THING",
		(char *)"thing",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_BEING",
		(char *)"being",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_CREATURE",
		(char *)"creature",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_PLAYER",
		(char *)"player",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_OBJECT",
		(char *)"object",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_PLACE",
		(char *)"place",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdor\\Entity\\TYPE_ROOM",
		(char *)"room",
		CONST_CS | CONST_PERSISTENT
	);

	return SUCCESS;
}

/*****************************************************************************/

PHP_MSHUTDOWN_FUNCTION(trogdor) {

	reapPersistedGames();
	return SUCCESS;
}

/*****************************************************************************/

zend_module_entry trogdor_module_entry = {

	#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
	#endif

	MODULE_NAME,
	NULL,                   /* Functions */
	PHP_MINIT(trogdor),
	PHP_MSHUTDOWN(trogdor),
	NULL,                   /* RINIT */
	NULL,                   /* RSHUTDOWN */
	PHP_MINFO(trogdor),

	#if ZEND_MODULE_API_NO >= 20010901
	PHP_TROGDOR_VERSION,
	#endif

	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TROGDOR
extern "C" {
	ZEND_GET_MODULE(trogdor)
}
#endif
