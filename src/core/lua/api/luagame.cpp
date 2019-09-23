#include "../../include/game.h"
#include "../../include/lua/api/luagame.h"

using namespace std;

namespace trogdor {


   // This is the metatable name for our global Lua Game object
   const char *LuaGame::MetatableName = "Game";

   // Non-object oriented functions that can't be called with the colon
   // operator or passed an instance of self as the first argument.
   static const luaL_Reg functions[] = {
      {0, 0}
   };

   // Lua Game methods that bind to C++ Game methods. These should be called
   // with the colon operator or passed an instance of self as the first
   // argument.
   static const luaL_Reg methods[] = {
      {"getTime", LuaGame::getTime},
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

      // This creates a metatable for operating on userdata (Game *)
      LuaState::luaL_register_wrapper(L, 0, methods);

      // This creates a SEPARATE package that just so happens to have the same
      // name as the metatable which function similar to static class methods in
      // C++
      LuaState::luaL_register_wrapper(L, MetatableName, functions);
   }

   /***************************************************************************/

   Game *LuaGame::checkGame(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Game **)luaL_checkudata(L, i, MetatableName);
   }

   /***************************************************************************/

   int LuaGame::getTime(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "method takes no arguments");
      }

      Game *g = checkGame(L, -1);

      if (0 == g) {
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
}

