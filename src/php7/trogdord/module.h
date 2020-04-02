#ifndef PHP_TROGDORD_MODULE_H
#define PHP_TROGDORD_MODULE_H

#define MODULE_NAME     "trogdord"

// A bit of mystical preprocessor wizardry
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// PHP Module version
#define PHP_TROGDORD_VERSION_MAJOR 0
#define PHP_TROGDORD_VERSION_MINOR 15
#define PHP_TROGDORD_VERSION_PATCH 0

#define PHP_TROGDORD_VERSION STR(PHP_TROGDORD_VERSION_MAJOR) "." STR(PHP_TROGDORD_VERSION_MINOR) "." STR(PHP_TROGDORD_VERSION_PATCH)

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
	#include "php.h"
	#include "ext/standard/info.h"
}

// This needs to be a true global
extern zend_module_entry trogdord_module_entry;

#endif /* PHP_TROGDORD_H */
