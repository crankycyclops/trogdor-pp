#include "player.h"

ZEND_DECLARE_MODULE_GLOBALS(player);

/*****************************************************************************/
/*****************************************************************************/

// Player Methods

// PHP Player class methods
static const zend_function_entry playerMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

void definePlayerClass() {

	zend_class_entry playerClass;

	INIT_CLASS_ENTRY(playerClass, "Trogdor\\Entity\\Player", playerMethods);
	PLAYER_GLOBALS(classEntry) = zend_register_internal_class_ex(&playerClass, BEING_GLOBALS(classEntry));

	// Player is a concrete entity type and can't be extended further
	PLAYER_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
