#include "creature.h"

ZEND_DECLARE_MODULE_GLOBALS(creature);

/*****************************************************************************/

// Creature Methods

// PHP Creature class methods
static const zend_function_entry creatureMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/

void defineCreatureClass() {

	zend_class_entry creatureClass;

	INIT_CLASS_ENTRY(creatureClass, "Trogdord\\Creature", creatureMethods);
	CREATURE_GLOBALS(classEntry) = zend_register_internal_class_ex(&creatureClass, BEING_GLOBALS(classEntry));

	// Creature is a concrete entity type and can't be extended further
	CREATURE_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
