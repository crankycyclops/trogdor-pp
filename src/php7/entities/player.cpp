#include "player.h"

ZEND_DECLARE_MODULE_GLOBALS(player);

/*****************************************************************************/
/*****************************************************************************/

// Player Methods

ZEND_BEGIN_ARG_INFO(arginfoPlayerCtor, 0)
	ZEND_ARG_INFO(0, XMLPath)
ZEND_END_ARG_INFO()

PHP_METHOD(Player, __construct) {

	php_error_docref(NULL, E_ERROR, "Don't instantiate Trogdor\\Entity\\Player directly! Instead, use Trogdor\\Game::createPlayer.");
	RETURN_NULL();
}

/*****************************************************************************/

// PHP Player class methods
static const zend_function_entry playerMethods[] =  {
	PHP_ME(Player, __construct, arginfoPlayerCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

// Utility functions

void refreshPlayerProperties(trogdor::entity::Entity *ePtr, zval *phpEntityObj) {

	// Placeholder in case I ever have player-specific properties to set in the
	// future
}

/*****************************************************************************/

void definePlayerClass() {

	zend_class_entry playerClass;

	INIT_CLASS_ENTRY(playerClass, "Trogdor\\Entity\\Player", playerMethods);
	PLAYER_GLOBALS(classEntry) = zend_register_internal_class_ex(&playerClass, BEING_GLOBALS(classEntry));

	// Player is a concrete entity type and can't be extended further
	PLAYER_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
