#include "../../../include/entities/room.h"
#include "../../../include/lua/api/entities/luaroom.h"

using namespace std;

namespace trogdor { namespace entity {


   // The name of the metatable that represents the Room metatable
   const char *LuaRoom::MetatableName = "Room";

   // This is the name of the library that contains functions related to Rooms
   // in the game
   const char *LuaRoom::PackageName = "Room";

   // Types which are considered valid by checkRoom()
   static const char *roomTypes[] = {
      "Room",
      0
   };

   // functions that take a Room as an input (new, get, etc.)
   // format of call: Room.new(e), where e is a Room
   static const luaL_Reg functions[] = {
      {0, 0}
   };

   // Lua Room methods that bind to C++ Room methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaRoom::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaRoom::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaRoom::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Room.__index = Room
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaPlace::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Room *LuaRoom::checkRoom(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Room **)LuaState::luaL_checkudata_ex(L, i, roomTypes);
   }
}}

