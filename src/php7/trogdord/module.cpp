#include <iostream>

#include "module.h"
#include "trogdord.h"
#include "game.h"
#include "exception.h"
#include "network/tcpconnectionmap.h"

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

	// Global namespaced constants that identify entity types and their string
	// representations. Note: I have to define these string constants here
	// instead of in a more appropriate location because the
	// REGISTER_STRING_CONSTANT macro relies on a variable which
	// PHP_MINIT_FUNCTION provides.
	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\ENTITY",
		(char *)"entity",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\PLACE",
		(char *)"place",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\THING",
		(char *)"thing",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\BEING",
		(char *)"being",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\ROOM",
		(char *)"room",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\OBJECT",
		(char *)"object",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\CREATURE",
		(char *)"creature",
		CONST_CS | CONST_PERSISTENT
	);

	REGISTER_STRING_CONSTANT(
		"Trogdord\\Entity\\Type\\PLAYER",
		(char *)"player",
		CONST_CS | CONST_PERSISTENT
	);

	return SUCCESS;
}

/*****************************************************************************/

PHP_MSHUTDOWN_FUNCTION(trogdord) {

	TCPConnectionMap::get().clear();
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
