#include "../../../include/entities/thing.h"
#include "../../../include/lua/api/entities/luathing.h"

using namespace std;

namespace core { namespace entity {


   // Types which are considered valid by checkThing()
   static const char *thingTypes[] = {
      "Thing",
      "Item",
      "Object",
      "Being",
      "Player",
      "Creature",
      0
   };

   // functions that take an Thing as an input (new, get, etc.)
   // format of call: Thing.new(e), where e is an Thing
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Thing methods that bind to C++ Thing methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {"addAlias", LuaThing::addAlias},
      {0, 0}
   };


   const luaL_reg *LuaThing::getFunctions() {

      return functions;
   }

   const luaL_reg *LuaThing::getMethods() {

      return methods;
   }

   void LuaThing::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Thing");

      // Entity.__index = Entity
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Thing", LuaEntity::getFunctions());
      luaL_register(L, "Thing", functions);
   }

   Thing *LuaThing::checkThing(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Thing **)LuaState::luaL_checkudata_ex(L, i, thingTypes);
   }

   int LuaThing::addAlias(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes one string argument");
      }

      Thing *t = checkThing(L, -2);

      if (0 == t) {
         return luaL_error(L, "not a Thing!");
      }

      string alias = luaL_checkstring(L, -1);
      t->addAlias(alias);

      return 1;
   }
}}

