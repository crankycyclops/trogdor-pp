#include <memory>

#include <trogdor/game.h>
#include <trogdor/entities/entity.h>
#include <trogdor/lua/api/luagame.h>

#include <trogdor/exception/exception.h>
#include <trogdor/exception/entityexception.h>

namespace trogdor {


   // This is the metatable name for our global Lua Game object
   const char *LuaGame::MetatableName = "Game";

   // This is the name of the library that contains functions related to the Lua
   // Game object
   const char *LuaGame::PackageName = "Game";

   // This is the variable name of the global instance of Game
   const char *LuaGame::globalName = "game";

   // Non-object oriented functions that can't be called with the colon
   // operator or passed an instance of self as the first argument.
   static const luaL_Reg functions[] = {
      {0, 0}
   };

   // Lua Game methods that bind to C++ Game methods. These should be called
   // with the colon operator or passed an instance of self as the first
   // argument.
   static const luaL_Reg methods[] = {
      {"insert",  LuaGame::insertEntity},
      {"getEntity", LuaGame::getEntity},
      {"getTime", LuaGame::getTime},
      {"start", LuaGame::start},
      {"stop", LuaGame::stop},
      {"inProgress", LuaGame::inProgress},
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaGame::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaGame::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaGame::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Game.__index = Game
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, methods);
      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Game *LuaGame::checkGame(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Game **)luaL_checkudata(L, i, MetatableName);
   }

   /***************************************************************************/

   int LuaGame::insertEntity(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "requires entity argument");
      }

      Game *g = checkGame(L, -2);
      entity::Entity *e = entity::LuaEntity::checkEntity(L, -1);

      try {

         // Hand the Game a shared_ptr whose deleter defers to
         // Entity::managedByLua(). If Game::insertEntity throws due to an entity
         // with the same name already existing, the shared_ptr is destroyed,
         // but the deleter will not free the entity because it's managed by Lua.
         // If this entity is deleted later in the game after having been handed
         // over, Entity::isManagedByLua() will return false and the entity will
         // be deleted as expected.
         std::shared_ptr<entity::Entity> owned(e, [](entity::Entity *ptr) {

            if (!ptr->isManagedByLua()) {
               delete ptr;
            }
         });

         g->insertEntity(e->getName(), owned);
         e->setManagedByLua(false);
         lua_pushboolean(L, 1);
      }

      // The entity was rejected due to a duplicate name, so the Lua function
      // returns false
      catch (const entity::EntityException &error) {
         lua_pushboolean(L, 0);
      }

      // Game related messages can appear directly in scripts for error handling
      catch (const Exception &error) {
         return luaL_error(L, "%s", error.what());
      }

      // Other lower level exceptions that are irrelevant to game or Lua
      // related issues will be reported on more generally
      catch (const std::exception &error) {
         return luaL_error(L, "internal engine error during game:insert()");
      }

      return 1;
   }

   /***************************************************************************/

   int LuaGame::getEntity(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "Requires exactly one argument: an entity name");
      }

      Game *g = checkGame(L, -2);
      std::string entityName = luaL_checkstring(L, -1);

      try {
         LuaState::pushEntity(L, g->getEntity(entityName).get());
      }

      catch (const entity::EntityException &e) {
         lua_pushboolean(L, false);
      }

      return 1;
   }

   /***************************************************************************/

   int LuaGame::getTime(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "method takes no arguments");
      }

      Game *g = checkGame(L, -1);

      if (nullptr == g) {
         return luaL_error(L, "Game object is nil");
      }

      // Lua 5.3 introduced an integer type
      #if LUA_VERSION_NUM > 502
         lua_pushinteger(L, g->getTime());
      #else
         lua_pushnumber(L, g->getTime());
      #endif

      return 1;
   }

   /***************************************************************************/

   int LuaGame::start(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "method takes no arguments");
      }

      Game *g = checkGame(L, -1);

      if (nullptr == g) {
         return luaL_error(L, "Game object is nil");
      }

      try {
         g->start();
      } catch (const trogdor::Exception &e) {
         luaL_error(L, e.what());
      }

      return 0;
   }

   /***************************************************************************/

   int LuaGame::stop(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "method takes no arguments");
      }

      Game *g = checkGame(L, -1);

      if (nullptr == g) {
         return luaL_error(L, "Game object is nil");
      }

      try {
         g->stop();
      } catch (const trogdor::Exception &e) {
         luaL_error(L, e.what());
      }

      return 0;
   }

   /***************************************************************************/

   int LuaGame::inProgress(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "method takes no arguments");
      }

      Game *g = checkGame(L, -1);

      if (nullptr == g) {
         return luaL_error(L, "Game object is nil");
      }

      lua_pushboolean(L, g->inProgress());
      return 1;
   }
}
