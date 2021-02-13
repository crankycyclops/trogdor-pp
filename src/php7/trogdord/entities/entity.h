#ifndef PHP_ENTITY_H
#define PHP_ENTITY_H

extern "C" {
	#include "php.h"
}

#include "../json.h"
#include "../compatibility.h"

// For an explanation of what I'm doing here, see:
// https://www.php.net/manual/en/internals2.structure.globals.php
ZEND_BEGIN_MODULE_GLOBALS(entity)
	zend_class_entry *classEntry;
ZEND_END_MODULE_GLOBALS(entity)

#ifdef ZTS
#define ENTITY_GLOBALS(v) TSRMG(entity_globals_id, entity_globals *, v)
#else
#define ENTITY_GLOBALS(v) (entity_globals.v)
#endif

// The private property which contains the entity's name
constexpr const char *NAME_PROPERTY_NAME = "name";

// The private property that contains reference to the game the entity belongs to
constexpr const char *GAME_PROPERTY_NAME = "game";

// True when the instance of \Trogdord\Entity is valid and false if the Entity it
// refers to no longer exists server-side.
constexpr const char *ENTITY_VALID_PROPERTY = "valid";

// String representations of each entity type
constexpr const char *ENTITY_TYPE_STR = "entity";
constexpr const char *TANGIBLE_TYPE_STR = "tangible";
constexpr const char *PLACE_TYPE_STR = "place";
constexpr const char *THING_TYPE_STR = "thing";
constexpr const char *BEING_TYPE_STR = "being";
constexpr const char *RESOURCE_TYPE_STR = "resource";
constexpr const char *ROOM_TYPE_STR = "room";
constexpr const char *OBJECT_TYPE_STR = "object";
constexpr const char *CREATURE_TYPE_STR = "creature";
constexpr const char *PLAYER_TYPE_STR = "player";

// Exception message when methods are called on a player that's already been destroyed
constexpr const char *ENTITY_ALREADY_DESTROYED = "Entity has already been destroyed";

// Retrieve the specified property for an instance of \Trogdord\Entity. Return
// type is zval *.
#define ENTITY_TO_PROP_VAL(THIS_PTR, RV, PROPERTY) zend_read_property(\
ENTITY_GLOBALS(classEntry), (THIS_PTR_COMPAT(THIS_PTR)), (PROPERTY), \
strlen((PROPERTY)), 1, (RV))

// Retrieve the game id from an instance of \Trogdord\Game.
#define ENTITY_TO_NAME(THIS_PTR, RV) ENTITY_TO_PROP_VAL(THIS_PTR, RV, NAME_PROPERTY_NAME)

// Retrieve the game id from an instance of \Trogdord\Game.
#define ENTITY_TO_GAME(THIS_PTR, RV) ENTITY_TO_PROP_VAL(THIS_PTR, RV, GAME_PROPERTY_NAME)

// Retrieve the "valid" property from an instance of \Trogdord\Entity.
#define ENTITY_IS_VALID_PROP(THIS_PTR, RV) ENTITY_TO_PROP_VAL(THIS_PTR, RV, ENTITY_VALID_PROPERTY)

// Validate the instance of \Trogdord\Entity before proceeding with an operation.
#define ASSERT_ENTITY_IS_VALID(ZVAL_VALID) \
if (IS_NULL == Z_TYPE_P(ZVAL_VALID)) { \
	zend_throw_exception(EXCEPTION_GLOBALS(entityNotFound), ENTITY_ALREADY_DESTROYED, 0); \
	RETURN_NULL(); \
}

/*****************************************************************************/

// Creates a PHP instance corresponding to the given trogdord entity. Returns
// true on success and false on error.
extern bool createEntityObj(zval *entityObj, Value &properties, zval *gameObj);

// Declares the PHP Entity class to the Zend engine.
extern void defineEntityClass();

#endif /* PHP_ENTITY_H */
