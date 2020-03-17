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

// String representations of each entity type
const char *ENTITY_TYPE_STR = "entity";
const char *PLACE_TYPE_STR = "place";
const char *THING_TYPE_STR = "thing";
const char *BEING_TYPE_STR = "being";
const char *ROOM_TYPE_STR = "room";
const char *OBJECT_TYPE_STR = "object";
const char *CREATURE_TYPE_STR = "creature";
const char *PLAYER_TYPE_STR = "player";

/*****************************************************************************/

// The constructor should NEVER be called in PHP userland. Instead, instances of
// Trogdord\Entity or their inherited classes will be returned by calls to
// methods defined in \Trogdord\Game.
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Entity, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
}

/*****************************************************************************/

// Magic "__get" allows us to make private and protected data members read-only.
ZEND_BEGIN_ARG_INFO(arginfoMagicGet, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Entity, __get) {

	char *key;
	size_t keyLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLength) == FAILURE) {
		RETURN_NULL()
	}

	zval rv; // ???
	zval *propVal = ENTITY_TO_PROP_VAL(getThis(), &rv, key);

	// There's some insanity in how this works, so for reference, here's what
	// I read to help me understand what all the arguments mean:
	// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
	RETURN_ZVAL(propVal, 1, 0);
}

/*****************************************************************************/

// PHP Entity class methods
static const zend_function_entry entityMethods[] =  {
	PHP_ME(Entity, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Entity, __get, arginfoMagicGet, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

bool createEntityObj(zval *entityObj, JSONObject properties, zval *gameObj) {

	std::string eType = properties.get<std::string>("type");
	std::string eName = properties.get<std::string>("name");

	zend_class_entry *eClassEntry;

	if (0 == eType.compare(ROOM_TYPE_STR)) {
		eClassEntry = ROOM_GLOBALS(classEntry);
	}

	else if (0 == eType.compare(OBJECT_TYPE_STR)) {
		eClassEntry = OBJECT_GLOBALS(classEntry);
	}

	else if (0 == eType.compare(CREATURE_TYPE_STR)) {
		eClassEntry = CREATURE_GLOBALS(classEntry);
	}

	else if (0 == eType.compare(PLAYER_TYPE_STR)) {
		eClassEntry = PLAYER_GLOBALS(classEntry);
	}

	else {
		throw std::runtime_error("Entity type '" + eType + "' not recognized. This is a bug.");
	}

	if (SUCCESS != object_init_ex(entityObj, eClassEntry)) {
		return false;
	}

	zend_update_property_string(
		eClassEntry,
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
