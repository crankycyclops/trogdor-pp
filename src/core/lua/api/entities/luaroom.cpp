#include "../../../include/entities/room.h"
#include "../../../include/lua/api/entities/luaroom.h"

using namespace std;

namespace core { namespace entity {


   // Types which are considered valid by checkRoom()
   static const char *roomTypes[] = {
      "Room",
      0
   };

   // functions that take a Room as an input (new, get, etc.)
   // format of call: Room.new(e), where e is a Room
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Room methods that bind to C++ Room methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };


   const luaL_reg *LuaRoom::getFunctions() {

      return functions;
   }

   const luaL_reg *LuaRoom::getMethods() {

      return methods;
   }

   void LuaRoom::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Room");

      // Room.__index = Room
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, LuaPlace::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Room", LuaEntity::getFunctions());
      luaL_register(L, "Room", LuaPlace::getFunctions());
      luaL_register(L, "Room", functions);
   }

   Room *LuaRoom::checkRoom(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Room **)LuaState::luaL_checkudata_ex(L, i, roomTypes);
   }
}}

