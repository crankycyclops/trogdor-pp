#include "../../../include/entities/creature.h"
#include "../../../include/lua/api/entities/luacreature.h"

using namespace std;

namespace trogdor { namespace entity {


   // The name of the metatable that represents the Creature metatable
   const char *LuaCreature::MetatableName = "Creature";

   // Types which are considered valid by checkCreature()
   static const char *creatureTypes[] = {
      "Creature",
      0
   };

   // functions that take a Creature as an input (new, get, etc.)
   // format of call: Creature.new(e), where e is a Creature
   static const luaL_Reg functions[] = {
      {0, 0}
   };

   // Lua Creature methods that bind to C++ Creature methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaCreature::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaCreature::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaCreature::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Creature.__index = Creature
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaThing::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaBeing::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);
      LuaState::luaL_register_wrapper(L, MetatableName, LuaEntity::getFunctions());
      LuaState::luaL_register_wrapper(L, MetatableName, LuaThing::getFunctions());
      LuaState::luaL_register_wrapper(L, MetatableName, LuaBeing::getFunctions());
      LuaState::luaL_register_wrapper(L, MetatableName, functions);
   }

   /***************************************************************************/

   Creature *LuaCreature::checkCreature(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Creature **)LuaState::luaL_checkudata_ex(L, i, creatureTypes);
   }
}}

