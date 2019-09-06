#include "../../../include/entities/creature.h"
#include "../../../include/lua/api/entities/luacreature.h"

using namespace std;

namespace trogdor { namespace core { namespace entity {


   // Types which are considered valid by checkCreature()
   static const char *creatureTypes[] = {
      "Creature",
      0
   };

   // functions that take a Creature as an input (new, get, etc.)
   // format of call: Creature.new(e), where e is a Creature
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Creature methods that bind to C++ Creature methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_reg *LuaCreature::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_reg *LuaCreature::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaCreature::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Creature");

      // Creature.__index = Creature
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, LuaThing::getMethods());
      luaL_register(L, 0, LuaBeing::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Creature", LuaEntity::getFunctions());
      luaL_register(L, "Creature", LuaThing::getFunctions());
      luaL_register(L, "Creature", LuaBeing::getFunctions());
      luaL_register(L, "Creature", functions);
   }

   /***************************************************************************/

   Creature *LuaCreature::checkCreature(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Creature **)LuaState::luaL_checkudata_ex(L, i, creatureTypes);
   }
}}}

