#include <iostream>
#include "module.h"

PHP_MINFO_FUNCTION(trogdord) {

	php_info_print_table_start();
	php_info_print_table_row(2, "trogdord", "enabled");
	php_info_print_table_row(2, "Extension Version", PHP_TROGDORD_VERSION);
	php_info_print_table_end();
}

/*****************************************************************************/

PHP_MINIT_FUNCTION(trogdord) {

	// TODO

	return SUCCESS;
}

/*****************************************************************************/

PHP_MSHUTDOWN_FUNCTION(trogdord) {

	// TODO

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
