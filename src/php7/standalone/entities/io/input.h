#ifndef PHP_ENTITY_INPUT_H
#define PHP_ENTITY_INPUT_H

#include <trogdor/entities/entity.h>

extern "C" {
	#include "php.h"
}

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(entityin)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(entityin)

#ifdef ZTS
#define ENTITYIN_GLOBALS(v) TSRMG(entityin_globals_id, entityin_globals *, v)
#else
#define ENTITYIN_GLOBALS(v) (entityin_globals.v)
#endif

// Custom data that will be instantiated alongside the zend_object
struct customInputData {
	trogdor::entity::Entity *ePtr;
};

struct inputDataWrapper {
	customInputData data;
	zend_object std;
};

// For an explanation of why this is necessary, see:
// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
inline inputDataWrapper *ZOBJ_TO_INPUTOBJ(zend_object *zobj) {

	return (inputDataWrapper *)((char *)(zobj) - XtOffsetOf(inputDataWrapper, std));
}

extern zend_object_handlers inputObjectHandlers;

/*****************************************************************************/

// Declares the PHP Trogdor\Entity\IO\Input class to the Zend engine.
extern void defineIOInputClass();

#endif /* PHP_ENTITY_INPUT_H */
