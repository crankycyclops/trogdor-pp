#include <vector>

#include <trogdor/game.h>

#include <trogdor/entities/tangible.h>
#include <trogdor/lua/api/entities/luatangible.h>

#include <trogdor/exception/exception.h>

namespace trogdor::entity {


   // The name of the metatable that represents the Tangible metatable
   const char *LuaTangible::MetatableName = "Tangible";

   // This is the name of the library that contains functions related to
   // Tangibles in the game
   const char *LuaTangible::PackageName = "Thing";

   // Types which are considered valid by checkTangible()
   static const char *tangibleTypes[] = {
      "Place",
      "Room",
      "Thing",
      "Object",
      "Being",
      "Player",
      "Creature",
      0
   };

   // functions that take a Tangible as an input (new, get, etc.)
   // format of call: Tangible.new(e), where e is a Tangible
   static const luaL_Reg functions[] = {
      {"get", LuaTangible::getTangible},
      {0, 0}
   };

   // Lua Tangible methods that bind to C++ Tangible methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaTangible::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaTangible::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaTangible::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Tangible.__index = Tangible
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Tangible *LuaTangible::checkTangible(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Tangible **)LuaState::luaL_checkudata_ex(L, i, tangibleTypes);
   }

   /***************************************************************************/

   int LuaTangible::getTangible(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Tangible *t = g->getTangible(name).get();

      if (t) {
         LuaState::pushEntity(L, t);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }
}
