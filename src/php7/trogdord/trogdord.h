#ifndef PHP_TROGDORD_H
#define PHP_TROGDORD_H

// If a port isn't specified when the Trogdord constructor is invoked, we assume
// that the daemon is running on this default port.
#define TROGDORD_DEFAULT_PORT 1040

extern "C" {
	#include "php.h"
}

#include "network/tcpconnection.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(trogdord)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(trogdord)

#ifdef ZTS
#define TROGDORD_GLOBALS(v) TSRMG(trogdord_globals_id, trogdord_globals *, v)
#else
#define TROGDORD_GLOBALS(v) (trogdord_globals.v)
#endif

// Custom data that will be instantiated alongside the zend_object
struct customData {

	// The hostname and port we're connecting to
	std::string hostname;
	unsigned short port;

	// Allows us to temporarily write to the read-only status property
	bool unlockStatusProperty = false;
};

// Allows us to wrap instance of PHP class \Trogdord with extra data
struct trogdordObject {
	customData data;
	zend_object std;
};

// For an explanation of why this is necessary, see:
// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
inline trogdordObject *ZOBJ_TO_TROGDORD(zend_object *zobj) {

	return (trogdordObject *)((char *)(zobj) - XtOffsetOf(trogdordObject, std));
}

extern zend_object_handlers trogdordObjectHandlers;

/*****************************************************************************/

// The read-only public property that gets set to status code returne by the
// response to the last request
constexpr const char *STATUS_PROPERTY = "status";

// Declares Trogdord class to the Zend Engine.
extern void defineTrogdordClass();

#endif /* PHP_TROGDORD_H */
