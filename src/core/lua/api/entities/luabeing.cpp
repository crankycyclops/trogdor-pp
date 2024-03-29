#include <trogdor/game.h>
#include <trogdor/entities/being.h>
#include <trogdor/lua/api/entities/luabeing.h>

namespace trogdor::entity {


   // The name of the metatable that represents the Being metatable
   const char *LuaBeing::MetatableName = "Being";

   // This is the name of the library that contains functions related to Beings
   // in the game
   const char *LuaBeing::PackageName = "Being";

   // Types that are considered valid by checkBeing()
   const char *LuaBeing::types[] = {
      "Being",
      "Player",
      "Creature",
      0
   };

   // functions that take a Being as an input (new, get, etc.)
   // format of call: Being.new(e), where e is a Being
   static const luaL_Reg functions[] = {
      {"get", LuaBeing::getBeing},
      {"insertIntoInventory", LuaBeing::insertIntoInventory},
      {"removeFromInventory", LuaBeing::removeFromInventory},
      {0, 0}
   };

   // Lua Being methods that bind to C++ Being methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaBeing::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaBeing::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaBeing::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Being.__index = Being
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaThing::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Being *LuaBeing::checkBeing(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Being **)LuaState::luaL_checkudata_ex(L, i, types);
   }

   /***************************************************************************/

   int LuaBeing::getBeing(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Being *b = g->getBeing(name).get();

      if (b) {
         LuaState::pushEntity(L, b);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }

   /***************************************************************************/

   int LuaBeing::insertIntoInventory(lua_State *L) {

      int n = lua_gettop(L);

      if (3 != n) {
         return luaL_error(L, "takes two arguments: the Being, the Object to insert, and whether or not to consider its weight (use colon for method call)");
      }

      bool considerWeight = lua_toboolean(L, -3);
      Object *o = LuaObject::checkObject(L, -2);
      Being *b = checkBeing(L, -1);

      if (nullptr == b) {
         return luaL_error(L, "calling object is not a Being");
      }

      else if (nullptr == o) {
         return luaL_error(L, "first argument is not an Object");
      }

      try {

         if (!b->insertIntoInventory(o->getShared(), considerWeight)) {
            return luaL_error(L, "object is too heavy to carry");
         } else {
            return 0;
         }
      }

      catch (const Exception &e) {
         return luaL_error(L, e.what());
      }
   }

   /***************************************************************************/

   int LuaBeing::removeFromInventory(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes two arguments: the Being and the Object to remove (use colon for method call)");
      }

      Object *o = LuaObject::checkObject(L, -2);
      Being *b = checkBeing(L, -1);

      if (nullptr == b) {
         return luaL_error(L, "calling object is not a Being");
      }

      else if (nullptr == o) {
         return luaL_error(L, "first argument is not an Object");
      }

      try {
         b->removeFromInventory(o->getShared());
         return 0;
      }

      catch (const Exception &e) {
         return luaL_error(L, e.what());
      }
   }
}
