#include <memory>

#include <trogdor/game.h>
#include <trogdor/entities/entity.h>
#include <trogdor/lua/api/luagame.h>

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
         g->insertEntity(e->getName(), std::shared_ptr<entity::Entity>(e));
         lua_pushboolean(L, 1);
      }

      catch (const entity::EntityException &e) {
         lua_pushboolean(L, 0);
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
