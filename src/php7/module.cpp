#include "module.h"
#include "game.h"
#include "exception.h"

PHP_MINFO_FUNCTION(trogdor) {

	// TODO
}

/*****************************************************************************/

PHP_MINIT_FUNCTION(trogdor) {

	defineExceptionClasses();
	defineGameClass();

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
	MODULE_VERSION,
	#endif

	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TROGDOR
extern "C" {
	ZEND_GET_MODULE(trogdor)
}
#endif
