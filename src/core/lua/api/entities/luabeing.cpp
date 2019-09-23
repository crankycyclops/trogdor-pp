#include "../../../include/entities/being.h"
#include "../../../include/lua/api/entities/luabeing.h"

using namespace std;

namespace trogdor { namespace entity {


   // The name of the metatable that represents the Being metatable
   const char *LuaBeing::MetatableName = "Being";

   // Types which are considered valid by checkBeing()
   static const char *beingTypes[] = {
      "Being",
      "Player",
      "Creature",
      0
   };

   // functions that take a Being as an input (new, get, etc.)
   // format of call: Being.new(e), where e is a Being
   static const luaL_Reg functions[] = {
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
      LuaState::luaL_register_wrapper(L, MetatableName, LuaEntity::getFunctions());
      LuaState::luaL_register_wrapper(L, MetatableName, LuaThing::getFunctions());
      LuaState::luaL_register_wrapper(L, MetatableName, functions);
   }

   /***************************************************************************/

   Being *LuaBeing::checkBeing(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Being **)LuaState::luaL_checkudata_ex(L, i, beingTypes);
   }
}}

