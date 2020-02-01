#ifndef PHP_TROGDOR_H
#define PHP_TROGDOR_H

#define MODULE_NAME     "trogdor"

// A bit of mystical preprocessor wizardry
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// PHP Module version
#define PHP_TROGDOR_VERSION_MAJOR 0
#define PHP_TROGDOR_VERSION_MINOR 1
#define PHP_TROGDOR_VERSION_PATCH 0

#define PHP_TROGOR_VERSION STR(#PHP_TROGDOR_VERSION_MAJOR "." #PHP_TROGDOR_VERSION_MINOR "." #PHP_TROGDOR_VERSION_PATCH)

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
	#include "php.h"
	#include "ext/standard/info.h"
}

// This needs to be a true global
extern zend_module_entry trogdor_module_entry;

#endif /* PHP_TROGDOR_H */
