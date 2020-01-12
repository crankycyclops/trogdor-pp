extern "C" {
	#include "php.h"
}

#include "game.h"

zend_class_entry *gameClassEntry;

// PHP Game class methods
static const zend_function_entry gameMethods[] =  {
	// TODO: define methods
	PHP_FE_END
};

/*****************************************************************************/

void defineGameClass() {

	zend_class_entry gameClass;

	INIT_CLASS_ENTRY(gameClass, "Trogdor\\Game", gameMethods);
	// gameClass.create_object = gameObjectNew; (TODO: DO I NEED THIS?)
	gameClassEntry = zend_register_internal_class(&gameClass);
}
