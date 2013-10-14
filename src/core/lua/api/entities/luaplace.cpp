#include "../../../include/entities/place.h"
#include "../../../include/lua/api/entities/luaplace.h"

using namespace std;

namespace core { namespace entity {


   // Types which are considered valid by checkPlace()
   static const char *placeTypes[] = {
      "Place",
      "Room",
      0
   };

   // functions that take a Place as an input (new, get, etc.)
   // format of call: Place.new(e), where e is a Place
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Place methods that bind to C++ Place methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };


   const luaL_reg *LuaPlace::getFunctions() {

      return functions;
   }

   const luaL_reg *LuaPlace::getMethods() {

      return methods;
   }

   void LuaPlace::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Place");

      // Place.__index = Place
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Place", LuaEntity::getFunctions());
      luaL_register(L, "Place", functions);
   }

   Place *LuaPlace::checkPlace(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Place **)LuaState::luaL_checkudata_ex(L, i, placeTypes);
   }
}}

