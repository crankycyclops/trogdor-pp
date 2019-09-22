#include "../../include/game.h"
#include "../../include/lua/api/luagame.h"

using namespace std;

namespace trogdor {


   // Non-object oriented functions that can't be called with the colon
   // operator or passed an instance of self as the first argument.
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Game methods that bind to C++ Game methods. These should be called
   // with the colon operator or passed an instance of self as the first
   // argument.
   static const luaL_reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_reg *LuaGame::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_reg *LuaGame::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaGame::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Game");

      // Game.__index = Game
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, methods);
      luaL_register(L, "Game", functions);
   }
}

