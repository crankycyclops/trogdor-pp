#include <trogdor/game.h>
#include <trogdor/entities/player.h>
#include <trogdor/lua/api/entities/luaplayer.h>

namespace trogdor::entity {


   // The name of the metatable that represents the Player metatable
   const char *LuaPlayer::MetatableName = "Player";

   // This is the name of the library that contains functions related to Players
   // in the game
   const char *LuaPlayer::PackageName = "Player";

   // Types which are considered valid by checkPlayer()
   static const char *playerTypes[] = {
      "Player",
      0
   };

   // functions that take a Player as an input (new, get, etc.)
   // format of call: Player.new(p), where p is a Player
   static const luaL_Reg functions[] = {
      {"get", LuaPlayer::getPlayer},
      {0, 0}
   };

   // Lua Player methods that bind to C++ Player methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaPlayer::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaPlayer::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaPlayer::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Player.__index = Player
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaThing::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaBeing::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Player *LuaPlayer::checkPlayer(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Player **)LuaState::luaL_checkudata_ex(L, i, playerTypes);
   }

   /***************************************************************************/

   int LuaPlayer::getPlayer(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Player *p = g->getPlayer(name).get();

      if (p) {
         LuaState::pushEntity(L, p);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }
}
