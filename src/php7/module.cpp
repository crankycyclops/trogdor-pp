// Look at C++ blog for example of how to allocate/free resources (constructor and destructor using object handlers)
#include "module.h"

PHP_MINIT_FUNCTION(trogdor) {

	// TODO
}

zend_module_entry trogdor_module_entry = {

	#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
	#endif

	MODULE_NAME,
	NULL,                  /* Functions */
	PHP_MINIT(trogdor),
	NULL,                  /* MSHUTDOWN */
	NULL,                  /* RINIT */
	NULL,                  /* RSHUTDOWN */
	NULL,                  /* MINFO */

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
