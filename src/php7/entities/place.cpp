#include "place.h"

ZEND_DECLARE_MODULE_GLOBALS(place);

/*****************************************************************************/
/*****************************************************************************/

// Place Methods

// PHP Place class methods
static const zend_function_entry placeMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

// Utility functions

void refreshPlaceProperties(trogdor::entity::Entity *ePtr, zval *phpEntityObj) {

	// TODO
}

/*****************************************************************************/

void definePlaceClass() {

	zend_class_entry placeClass;

	INIT_CLASS_ENTRY(placeClass, "Trogdor\\Entity\\Place", placeMethods);
	PLACE_GLOBALS(classEntry) = zend_register_internal_class_ex(&placeClass, ENTITY_GLOBALS(classEntry));
	PLACE_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
}
