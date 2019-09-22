#include "../../../include/entities/player.h"
#include "../../../include/lua/api/entities/luaplayer.h"

using namespace std;

namespace trogdor { namespace entity {


   // The name of the metatable that represents the Player metatable
   const char *LuaPlayer::MetatableName = "Player";

   // Types which are considered valid by checkPlayer()
   static const char *playerTypes[] = {
      "Player",
      0
   };

   // functions that take a Player as an input (new, get, etc.)
   // format of call: Player.new(p), where p is a Player
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Player methods that bind to C++ Player methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_reg *LuaPlayer::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_reg *LuaPlayer::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaPlayer::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Player.__index = Player
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, LuaThing::getMethods());
      luaL_register(L, 0, LuaBeing::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, MetatableName, LuaEntity::getFunctions());
      luaL_register(L, MetatableName, LuaThing::getFunctions());
      luaL_register(L, MetatableName, LuaBeing::getFunctions());
      luaL_register(L, MetatableName, functions);
   }

   /***************************************************************************/

   Player *LuaPlayer::checkPlayer(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Player **)LuaState::luaL_checkudata_ex(L, i, playerTypes);
   }
}}

