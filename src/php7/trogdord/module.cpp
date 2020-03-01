#include "module.h"

PHP_MINFO_FUNCTION(trogdor) {

	php_info_print_table_start();
	php_info_print_table_row(2, "trogdord", "enabled");
	php_info_print_table_row(2, "Extension Version", PHP_TROGDORD_VERSION);
	php_info_print_table_end();
}

/*****************************************************************************/

PHP_MINIT_FUNCTION(trogdor) {

	// TODO

	return SUCCESS;
}

/*****************************************************************************/

PHP_MSHUTDOWN_FUNCTION(trogdor) {

	// TODO

	return SUCCESS;
}

/*****************************************************************************/

zend_module_entry trogdord_module_entry = {

	STANDARD_MODULE_HEADER,
	MODULE_NAME,
	NULL,                   /* Functions */
	PHP_MINIT(trogdor),
	PHP_MSHUTDOWN(trogdor),
	NULL,                   /* RINIT */
	NULL,                   /* RSHUTDOWN */
	PHP_MINFO(trogdor),
	PHP_TROGDORD_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TROGDOR
extern "C" {
	ZEND_GET_MODULE(trogdor)
}
#endif
