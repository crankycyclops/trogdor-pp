#include "../game.h"
#include "../trogdord.h"

#include "entity.h"

#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"

ZEND_DECLARE_MODULE_GLOBALS(entity);

ZEND_EXTERN_MODULE_GLOBALS(trogdord);
ZEND_EXTERN_MODULE_GLOBALS(game);
ZEND_EXTERN_MODULE_GLOBALS(room);
ZEND_EXTERN_MODULE_GLOBALS(object);
ZEND_EXTERN_MODULE_GLOBALS(creature);
ZEND_EXTERN_MODULE_GLOBALS(player);

// The private property which contains the entity's name
const char *ENTITY_PROPERTY_NAME = "name";

const std::unordered_map<std::string, zend_class_entry *> EntityTypes = {
	{"room",     ROOM_GLOBALS(classEntry)},
	{"object",   OBJECT_GLOBALS(classEntry)},
	{"creature", CREATURE_GLOBALS(classEntry)},
	{"player",   PLAYER_GLOBALS(classEntry)},
};

/*****************************************************************************/

// Entity Methods

// PHP Entity class methods
static const zend_function_entry entityMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/

bool createEntityObj(zval *entityObj, JSONObject properties, zval *gameObj) {

	std::string eType = properties.get<std::string>("type");
	std::string eName = properties.get<std::string>("name");

	if (EntityTypes.end() == EntityTypes.find(eType)) {
		throw std::runtime_error("Entity type '" + eType + "' not recognized. This is a bug.");
	}

	if (SUCCESS != object_init_ex(entityObj, EntityTypes.find(eType)->second)) {
		return false;
	}

	zend_update_property_string(
		EntityTypes.find(eType)->second,
		entityObj,
		ENTITY_PROPERTY_NAME,
		strlen(ENTITY_PROPERTY_NAME),
		eName.c_str()
	);

	return true;
}

/*****************************************************************************/

void defineEntityClass() {

	zend_class_entry entityClass;

	INIT_CLASS_ENTRY(entityClass, "Trogdord\\Entity", entityMethods);
	ENTITY_GLOBALS(classEntry) = zend_register_internal_class(&entityClass);
	ENTITY_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	// Declare the Entity class's properties
	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"name",
		sizeof("name") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);
}
