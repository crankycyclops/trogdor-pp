#ifndef PHP_TROGDOR_H
#define PHP_TROGDOR_H

#define MODULE_NAME     "trogdor"
#define MODULE_VERSION  "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
	#include "php.h"
}

extern zend_module_entry trogdor_module_entry;

#endif /* PHP_TROGDOR_H */
