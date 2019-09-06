#include "../../../include/entities/being.h"
#include "../../../include/lua/api/entities/luabeing.h"

using namespace std;

namespace trogdor { namespace core { namespace entity {


   // Types which are considered valid by checkBeing()
   static const char *beingTypes[] = {
      "Being",
      "Player",
      "Creature",
      0
   };

   // functions that take a Being as an input (new, get, etc.)
   // format of call: Being.new(e), where e is a Being
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Being methods that bind to C++ Being methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_reg *LuaBeing::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_reg *LuaBeing::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaBeing::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Being");

      // Being.__index = Being
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, LuaThing::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Being", LuaEntity::getFunctions());
      luaL_register(L, "Being", LuaThing::getFunctions());
      luaL_register(L, "Being", functions);
   }

   /***************************************************************************/

   Being *LuaBeing::checkBeing(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Being **)LuaState::luaL_checkudata_ex(L, i, beingTypes);
   }
}}}

