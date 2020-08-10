#include <vector>

#include <trogdor/game.h>

#include <trogdor/entities/resource.h>
#include <trogdor/lua/api/entities/luaresource.h>

#include <trogdor/exception/exception.h>

namespace trogdor::entity {


   // The name of the metatable that represents the Resource metatable
   const char *LuaResource::MetatableName = "Resource";

   // This is the name of the library that contains functions related to
   // Resources in the game
   const char *LuaResource::PackageName = "Resource";

   // Types which are considered valid by checkResource()
   static const char *resourceTypes[] = {
      "Resource",
      0
   };

   // functions that take a Resource as an input (new, get, etc.)
   // format of call: Resource.new(e), where e is a Resource
   static const luaL_Reg functions[] = {
      {"get", LuaResource::getResource},
      {0, 0}
   };

   // Lua Resource methods that bind to C++ Resource methods. Also includes meta
   // methods.
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaResource::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaResource::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaResource::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Resource.__index = Resource
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Resource *LuaResource::checkResource(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Resource **)LuaState::luaL_checkudata_ex(L, i, resourceTypes);
   }

   /***************************************************************************/

   int LuaResource::getResource(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Resource *r = g->getResource(name).get();

      if (r) {
         LuaState::pushEntity(L, r);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }
}
