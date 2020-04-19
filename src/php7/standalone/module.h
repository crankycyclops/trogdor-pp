#ifndef PHP_TROGDOR_H
#define PHP_TROGDOR_H

#include <trogdor/version.h>

#define MODULE_NAME     "trogdor"

// A bit of mystical preprocessor wizardry
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// PHP Module version
#define PHP_TROGDOR_VERSION_MAJOR 0
#define PHP_TROGDOR_VERSION_MINOR 4
#define PHP_TROGDOR_VERSION_PATCH 1

#define PHP_TROGDOR_VERSION STR(PHP_TROGDOR_VERSION_MAJOR) "." STR(PHP_TROGDOR_VERSION_MINOR) "." STR(PHP_TROGDOR_VERSION_PATCH)

// Core library version string
#define TROGDOR_VERSION STR(TROGDOR_VERSION_MAJOR) "." STR(TROGDOR_VERSION_MINOR) "." STR(TROGDOR_VERSION_PATCH)

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
