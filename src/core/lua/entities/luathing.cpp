#include "../../include/entities/thing.h"
#include "../../include/lua/entities/luathing.h"

using namespace std;

namespace core { namespace entity {


   // functions that take an Thing as an input (new, get, etc.)
   // format of call: Thing.new(e), where e is an Thing
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Thing methods that bind to C++ Thing methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };


   const luaL_reg *LuaThing::getFunctions() {

      return functions;
   }

   const luaL_reg *LuaThing::getMethods() {

      return methods;
   }

   void LuaThing::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Entity");

      // Entity.__index = Entity
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Thing", LuaEntity::getFunctions());
      luaL_register(L, "Thing", functions);
   }
}}

