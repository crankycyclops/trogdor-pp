#include "game.h"
#include "module.h"
#include "exception.h"
#include "streamerr.h"

#include <iostream>

ZEND_DECLARE_MODULE_GLOBALS(game);

zend_object_handlers gameObjectHandlers;

// Indices in persistedGames that have been reclaimed (had their Game pointers
// set to nullptr)
static std::vector<size_t> recycledGameIds;

// Persisted games that can later be retrived by index
static std::vector<trogdor::Game *> persistedGames;

/*****************************************************************************/
/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createGameObject(zend_class_entry *classEntry TSRMLS_DC) {

	gameObject *gObj = (gameObject *)ecalloc(1, sizeof(*gObj) + zend_object_properties_size(classEntry));

	gObj->realGameObject.persistent = false;
	gObj->realGameObject.id = 0;
	gObj->realGameObject.obj = new trogdor::Game(
		std::make_unique<PHPStreamErr>()
	);

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

static void freeGameObject(zend_object *object TSRMLS_DC) {

	// If the game hasn't been persisted, it should be freed immediately
	if (!ZOBJ_TO_GAMEOBJ(object)->realGameObject.persistent) {
		delete ZOBJ_TO_GAMEOBJ(object)->realGameObject.obj;
		std::cout << "Destroyed!" << std::endl;
	}

	zend_object_std_dtor(object TSRMLS_CC);
}

/*****************************************************************************/
/*****************************************************************************/

// Game methods

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
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.obj;

	// Parse the XML game definition
	std::unique_ptr<trogdor::XMLParser> parser = std::make_unique<trogdor::XMLParser>(
		gameObjPtr->makeInstantiator(), gameObjPtr->getVocabulary()
	);

	try {
		gameObjPtr->initialize(parser.get(), Z_STRVAL_P(XMLPath));
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
	zend_update_property_long(GAME_GLOBALS(gameClassEntry), thisPtr, "id", sizeof("id") - 1, id TSRMLS_DC);

	RETURN_LONG(id);
}

/*****************************************************************************/

PHP_METHOD(Game, depersist) {

	zval *zId, rv;
	zval *thisPtr = getThis();

	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "expects no arguments");
	}

	zId = zend_read_property(GAME_GLOBALS(gameClassEntry), thisPtr, "id", sizeof("id") - 1, 1, &rv TSRMLS_CC);

	if (Z_TYPE_P(zId) == IS_LONG) {
		size_t id = Z_LVAL_P(zId);
		depersistGame(Z_LVAL_P(zId));
		ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.id = 0;
		ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject.persistent = false;
		zend_update_property_null(GAME_GLOBALS(gameClassEntry), thisPtr, "id", sizeof("id") - 1 TSRMLS_CC);
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
	PHP_ME(Game,     __construct, arginfoGameCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Game, getPersistentId, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,         persist, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,       depersist, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,           start, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,            stop, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,         getTime, NULL, ZEND_ACC_PUBLIC)
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

	persistedGames.push_back(game);
	return persistedGames.size() - 1;
}

/*****************************************************************************/

// Depersist a game referenced by its id. Only call this from the PHP depersist
// method! If you depersist a Game object after its corresponding PHP class has
// been destroyed, you'll have a memory leak.
void depersistGame(size_t id) {

	if (persistedGames.size() > id && nullptr != persistedGames[id]) {
		persistedGames[id] = nullptr;
		recycledGameIds.push_back(id);
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
	GAME_GLOBALS(gameClassEntry) = zend_register_internal_class(&gameClass);

	// Make sure users can't extend the Game class
	GAME_GLOBALS(gameClassEntry)->ce_flags |= ZEND_ACC_FINAL;

	// Declare the Game class's properties
	zend_declare_property_null(GAME_GLOBALS(gameClassEntry), "id", sizeof("id") - 1, ZEND_ACC_PRIVATE TSRMLS_CC);

	// Start out with default object handlers
	memcpy(&gameObjectHandlers, zend_get_std_object_handlers(), sizeof(gameObjectHandlers));

	// Set the specific custom object handlers we need
	GAME_GLOBALS(gameClassEntry)->create_object = createGameObject;
	gameObjectHandlers.free_obj = freeGameObject;
	gameObjectHandlers.dtor_obj = destroyGameObject;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	gameObjectHandlers.offset = XtOffsetOf(gameObject, std);
}
