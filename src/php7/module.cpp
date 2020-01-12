// Look at C++ blog for example of how to allocate/free resources (constructor and destructor using object handlers)
#include "module.h"
#include "game.h"

PHP_MINFO_FUNCTION(trogdor) {

	// TODO
}

/*****************************************************************************/

PHP_MINIT_FUNCTION(trogdor) {

	defineGameClass();
}

/*****************************************************************************/

PHP_MSHUTDOWN_FUNCTION(trogdor) {

	// TODO
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
