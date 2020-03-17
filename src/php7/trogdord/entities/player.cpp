#include "player.h"

ZEND_DECLARE_MODULE_GLOBALS(player);

/*****************************************************************************/

// Removes the player from the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues. If the game the player belongs to no longer exists,
// \TrogdordGameNotFound will be thrown. If the player has already been
// destroyed, \Trogdord\PlayerNotFound will be thrown.
ZEND_BEGIN_ARG_INFO(arginfoDestroy, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Player, destroy) {

	// TODO
	RETURN_NULL();
}

/*****************************************************************************/

// PHP Player class methods
static const zend_function_entry playerMethods[] =  {
	PHP_ME(Player, destroy, arginfoDestroy, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

void definePlayerClass() {

	zend_class_entry playerClass;

	INIT_CLASS_ENTRY(playerClass, "Trogdord\\Player", playerMethods);
	PLAYER_GLOBALS(classEntry) = zend_register_internal_class_ex(&playerClass, BEING_GLOBALS(classEntry));

	// Player is a concrete entity type and can't be extended further
	PLAYER_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
