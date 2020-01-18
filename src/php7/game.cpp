#include "game.h"
#include "module.h"
#include "exception.h"

ZEND_DECLARE_MODULE_GLOBALS(game);

zend_object_handlers gameObjectHandlers;

// Indices in persistedGames that have been reclaimed (had their Game pointers
// set to nullptr)
static std::queue<size_t> recycledGameIds;

// Persisted games that can later be retrived by index
static std::vector<trogdor::Game *> persistedGames;

// Maps trogdor::Game * to a struct containing data that can't be stored in
// customData.
std::unordered_map<
	trogdor::Game *,
	std::unique_ptr<customConstructedData>,
	std::hash<trogdor::Game *>
> customConstructedDataMap;

/*****************************************************************************/
/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createGameObject(zend_class_entry *classEntry TSRMLS_DC) {

	gameObject *gObj = (gameObject *)ecalloc(1, sizeof(*gObj) + zend_object_properties_size(classEntry));

	// We'll either initialize this with a new instance of trogdor::Game in
	// Trogdor\Game::__construct() or assign it an existing instance in
	// Trogdor\Game::get().
	gObj->realGameObject.persistent = false;
	gObj->realGameObject.id = 0;
	gObj->realGameObject.obj = nullptr;

	zend_object_std_init(&gObj->std, classEntry);
	object_properties_init(&gObj->std, classEntry);

	gObj->std.handlers = &gameObjectHandlers;

	return &gObj->std;
}

/*****************************************************************************/

static void destroyGameObject(zend_object *object TSRMLS_DC) {

	// If the game is running and hasn't been persisted, be polite and stop it
	// before it's freed
	if (!ZOBJ_TO_GAMEOBJ(object)->realGameObject.persistent) {
		ZOBJ_TO_GAMEOBJ(object)->realGameObject.obj->stop();
	}

	zend_objects_destroy_object(object);
}

/*****************************************************************************/

static void freeGameObject(zend_object *object TSRMLS_DC) {

	// If the game hasn't been persisted, it should be freed immediately
	if (!ZOBJ_TO_GAMEOBJ(object)->realGameObject.persistent) {
		customConstructedDataMap.erase(ZOBJ_TO_GAMEOBJ(object)->realGameObject.obj);
		delete ZOBJ_TO_GAMEOBJ(object)->realGameObject.obj;
	}

	zend_object_std_dtor(object TSRMLS_CC);
}

/*****************************************************************************/
/*****************************************************************************/

// Game methods

ZEND_BEGIN_ARG_INFO(arginfoGameGet, 0)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, get) {

	size_t id;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "l", &id) == FAILURE) {
		php_error_docref(NULL, E_ERROR, "expected exactly 1 parameter (Persistent Game object id)");
	}

	trogdor::Game *gameObj = getGameById(id);

	// Instantiate a new Trogdor\Game object and assign to it the persisted
	// instange of trogdor::Game.
	if (nullptr != gameObj) {

		// Note: return_value is a global defined somewhere by PHP. I spent
		// some time banging my head against the wall until I realized that.
		if (SUCCESS != object_init_ex(return_value, GAME_GLOBALS(classEntry))) {
			RETURN_NULL();
		}

		ZOBJ_TO_GAMEOBJ(Z_OBJ_P(return_value))->realGameObject.persistent = true;
		ZOBJ_TO_GAMEOBJ(Z_OBJ_P(return_value))->realGameObject.id = id;
		ZOBJ_TO_GAMEOBJ(Z_OBJ_P(return_value))->realGameObject.obj = gameObj;

		// Create a new instance of customConstructedData that will contain data
		// that can't be stored in gameObject.customData.
		if (customConstructedDataMap.end() == customConstructedDataMap.find(gameObj)) {
			customConstructedDataMap[gameObj] = std::make_unique<customConstructedData>();
		}

		zend_update_property_long(GAME_GLOBALS(classEntry), return_value, "persistentId", sizeof("persistentId") - 1, id TSRMLS_DC);
	}

	else {
		RETURN_NULL();
	}
}

/*****************************************************************************/

ZEND_BEGIN_ARG_INFO(arginfoGameCtor, 0)
	ZEND_ARG_INFO(0, XMLPath)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, __construct) {

	zval *XMLPath;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "z", &XMLPath) == FAILURE) {
		php_error_docref(NULL, E_ERROR, "expected exactly 1 parameter (valid path to a game file)");
	}

	if (Z_TYPE_P(XMLPath) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "expected string");
		convert_to_string(XMLPath);
	}

	zval *thisPtr = getThis();
	gameObject *gObj = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr));

	// Create a new instance of trogdor::Game
	gObj->realGameObject.obj = new trogdor::Game(
		std::make_unique<PHPStreamErr>()
	);

	// Create a new instance of customConstructedData that will contain data
	// that can't be stored in gameObject.customData.
	customConstructedDataMap[gObj->realGameObject.obj] = std::make_unique<customConstructedData>();

	// Parse the XML game definition
	std::unique_ptr<trogdor::XMLParser> parser = std::make_unique<trogdor::XMLParser>(
		gObj->realGameObject.obj->makeInstantiator(), gObj->realGameObject.obj->getVocabulary()
	);

	try {
		gObj->realGameObject.obj->initialize(parser.get(), Z_STRVAL_P(XMLPath));
	}

	catch (trogdor::ValidationException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(validationException), e.what(), 0);
	}

	catch (trogdor::UndefinedException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(undefinedException), e.what(), 0);
	}

	catch (trogdor::ParseException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(parseException), e.what(), 0);
	}

	catch (trogdor::LuaException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(luaException), e.what(), 0);
	}

	catch (trogdor::entity::EntityException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(entityException), e.what(), 0);
	}

	catch (trogdor::entity::BeingException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(beingException), e.what(), 0);
	}

	catch (trogdor::Exception &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(baseException), e.what(), 0);
	}
}

/*****************************************************************************/

PHP_METHOD(Game, getPersistentId) {

	zval *thisPtr = getThis();
	customData objData = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject;

	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "expects no arguments");
	}

	if (objData.persistent) {
		RETURN_LONG(objData.id);
	} else {
		RETURN_NULL();
	}
}

/*****************************************************************************/

PHP_METHOD(Game, persist) {

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "expects no arguments");
	}

	size_t id = persistGame(gameObjPtr);

	ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.id = id;
	ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.persistent = true;

	zend_update_property_long(GAME_GLOBALS(classEntry), thisPtr, "persistentId", sizeof("persistentId") - 1, id TSRMLS_DC);

	RETURN_LONG(id);
}

/*****************************************************************************/

PHP_METHOD(Game, depersist) {

	zval *zId, rv;
	zval *thisPtr = getThis();

	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "expects no arguments");
	}

	zId = zend_read_property(GAME_GLOBALS(classEntry), thisPtr, "persistentId", sizeof("persistentId") - 1, 1, &rv TSRMLS_CC);

	if (Z_TYPE_P(zId) == IS_LONG) {
		size_t id = Z_LVAL_P(zId);
		depersistGame(Z_LVAL_P(zId));
		ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.id = 0;
		ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.persistent = false;
		zend_update_property_null(GAME_GLOBALS(classEntry), thisPtr, "persistentId", sizeof("persistentId") - 1 TSRMLS_CC);
	}

	else {
		php_error_docref(NULL, E_WARNING, "cannot depersist an already non-persisted game");
	}

	RETURN_NULL();
}

/*****************************************************************************/

PHP_METHOD(Game, start) {

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "expects no arguments");
	}

	try {
		gameObjPtr->start();
	}

	catch (trogdor::Exception &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(baseException), e.what(), 0);
	}

	RETURN_NULL()
}

/*****************************************************************************/

PHP_METHOD(Game, stop) {

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "expects no arguments");
	}

	try {
		gameObjPtr->stop();
	}

	catch (trogdor::Exception &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(baseException), e.what(), 0);
	}

	RETURN_NULL()
}

/*****************************************************************************/

PHP_METHOD(Game, getTime) {

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "expects no arguments");
	}

	RETURN_LONG(gameObjPtr->getTime());
}

/*****************************************************************************/

ZEND_BEGIN_ARG_INFO(arginfoGameGetMeta, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getMeta) {

	char *key;
	size_t keyLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLength) == FAILURE) {
		RETURN_NULL();
	}

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	RETURN_STRING(gameObjPtr->getMeta(key).c_str());
}

/*****************************************************************************/

ZEND_BEGIN_ARG_INFO(arginfoGameGetEntity, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getEntity) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	trogdor::entity::Entity *ePtr = gameObjPtr->getEntity(name);

	if (ePtr) {

		switch (ePtr->getType()) {

			case ENTITY_ROOM:

				if (SUCCESS != object_init_ex(return_value, ROOM_GLOBALS(classEntry))) {
					RETURN_NULL();
				}

				break;

			case ENTITY_OBJECT:

				if (SUCCESS != object_init_ex(return_value, OBJECT_GLOBALS(classEntry))) {
					RETURN_NULL();
				}

				break;

			case ENTITY_CREATURE:

				if (SUCCESS != object_init_ex(return_value, CREATURE_GLOBALS(classEntry))) {
					RETURN_NULL();
				}

				break;

			case ENTITY_PLAYER:

				if (SUCCESS != object_init_ex(return_value, PLAYER_GLOBALS(classEntry))) {
					RETURN_NULL();
				}

				break;

			default:

				php_error_docref(NULL, E_WARNING, "Found unknown entity type (this is probably a bug.) Returning null instead.");
				RETURN_NULL();
		}

		ZOBJ_TO_ENTITYOBJ(Z_OBJ_P(return_value))->realEntityObject.obj = ePtr;
		ZOBJ_TO_ENTITYOBJ(Z_OBJ_P(return_value))->realEntityObject.managedByGame = true;

		zval outputObj;

		if (SUCCESS != object_init_ex(&outputObj, ENTITYOUT_GLOBALS(classEntry))) {
			php_error_docref(NULL, E_ERROR, "Could not instantiate Trogdor\\Entity\\IO\\Output. This is a bug.");
		}

		// The output buffer needs a pointer to the Entity it's assigned to so
		// it can key into the output buffer.
		ZOBJ_TO_OUTPUTOBJ(Z_OBJ(outputObj))->data.ePtr = ePtr;

		// Temporarily make output writeable.
		entityObjectHandlers.write_property = zend_std_write_property;

		// This read-only property is the object we use to consume messages
		// from the output buffer.
		zend_update_property(
			ENTITY_GLOBALS(classEntry),
			return_value,
			"output",
			sizeof("output") - 1,
			&outputObj TSRMLS_DC
		);

		// Once we've updated the output property, make it read-only again so
		// it can't be modified from PHP userland.
		entityObjectHandlers.write_property = writeProperty;

		zval_ptr_dtor(&outputObj);

		zend_update_property_string(
			ENTITY_GLOBALS(classEntry),
			return_value,
			"name",
			sizeof("name") - 1,
			ePtr->getName().c_str() TSRMLS_DC
		);

		zend_update_property_string(
			ENTITY_GLOBALS(classEntry),
			return_value,
			"title",
			sizeof("title") - 1,
			ePtr->getTitle().c_str() TSRMLS_DC
		);

		zend_update_property_string(
			ENTITY_GLOBALS(classEntry),
			return_value,
			"longDesc",
			sizeof("longDesc") - 1,
			ePtr->getLongDescription().c_str() TSRMLS_DC
		);

		zend_update_property_string(
			ENTITY_GLOBALS(classEntry),
			return_value,
			"shortDesc",
			sizeof("shortDesc") - 1,
			ePtr->getShortDescription().c_str() TSRMLS_DC
		);
	}

	// Entity by the given name doesn't exist in the game, so just return null
	else {
		RETURN_NULL();
	}
}

/*****************************************************************************/

ZEND_BEGIN_ARG_INFO(arginfoGameSetMeta, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, setMeta) {

	char *key;
	size_t keyLength;

	char *value;
	size_t valueLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &keyLength,
	&value, &valueLength) == FAILURE) {
		RETURN_NULL();
	}

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	gameObjPtr->setMeta(key, value);

	RETURN_NULL();
}

/*****************************************************************************/

// PHP Game class methods
static const zend_function_entry gameMethods[] =  {
	PHP_ME(Game,             get, arginfoGameGet, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(Game,     __construct, arginfoGameCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Game, getPersistentId, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,         persist, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,       depersist, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,           start, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,            stop, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,         getTime, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,       getEntity, arginfoGameGetEntity, ZEND_ACC_PUBLIC)
	PHP_ME(Game,         getMeta, arginfoGameGetMeta, ZEND_ACC_PUBLIC)
	PHP_ME(Game,         setMeta, arginfoGameSetMeta, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

// Retrive a persisted game by index
extern trogdor::Game *getGameById(size_t id) {

	return persistedGames.size() > id ? persistedGames[id] : nullptr;
}

/*****************************************************************************/

// Persist a game and return its id. This function does not check if a Game *
// has already been persisted. Instead, that check should occur in the PHP
// "persist" method.
size_t persistGame(trogdor::Game *game) {

	if (recycledGameIds.size()) {
		size_t i = recycledGameIds.front();
		recycledGameIds.pop();
		persistedGames[i] = game;
		return i;
	}

	else {
		persistedGames.push_back(game);
		return persistedGames.size() - 1;
	}
}

/*****************************************************************************/

// Depersist a game referenced by its id. Only call this from the PHP depersist
// method! If you depersist a Game object after its corresponding PHP class has
// been destroyed, you'll have a memory leak.
void depersistGame(size_t id) {

	if (persistedGames.size() > id && nullptr != persistedGames[id]) {
		persistedGames[id] = nullptr;
		recycledGameIds.push(id);
	}
}

/*****************************************************************************/

// Call this during MSHUTDOWN to delete all remaining Game pointers.
void reapPersistedGames() {

	while (!persistedGames.empty()) {

		if (nullptr != persistedGames.back()) {
			delete persistedGames.back();
		}

		persistedGames.pop_back();
	}
}

/*****************************************************************************/

void defineGameClass() {

	zend_class_entry gameClass;

	INIT_CLASS_ENTRY(gameClass, "Trogdor\\Game", gameMethods);
	GAME_GLOBALS(classEntry) = zend_register_internal_class(&gameClass);

	// Make sure users can't extend the Game class
	GAME_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;

	// Declare the Game class's properties
	zend_declare_property_null(GAME_GLOBALS(classEntry), "persistentId", sizeof("persistentId") - 1, ZEND_ACC_PRIVATE TSRMLS_CC);

	// Start out with default object handlers
	memcpy(&gameObjectHandlers, zend_get_std_object_handlers(), sizeof(gameObjectHandlers));

	// Set the specific custom object handlers we need
	GAME_GLOBALS(classEntry)->create_object = createGameObject;
	gameObjectHandlers.free_obj = freeGameObject;
	gameObjectHandlers.dtor_obj = destroyGameObject;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	gameObjectHandlers.offset = XtOffsetOf(gameObject, std);
}
