#include <vector>

#include "../../../include/entities/thing.h"
#include "../../../include/entities/place.h"
#include "../../../include/lua/api/entities/luathing.h"
#include "../../../include/lua/api/entities/luaplace.h"

using namespace std;

namespace trogdor { namespace entity {


   // Types which are considered valid by checkThing()
   static const char *thingTypes[] = {
      "Thing",
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
      {"getAliases",    LuaThing::getAliases},
      {"addAlias",      LuaThing::addAlias},
      {"getLocation",   LuaThing::getLocation},
      {"setLocation",   LuaThing::setLocation},
      {"clearLocation", LuaThing::clearLocation},
      {0, 0}
   };

   /***************************************************************************/

   const luaL_reg *LuaThing::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_reg *LuaThing::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaThing::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Thing");

      // Thing.__index = Thing
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Thing", LuaEntity::getFunctions());
      luaL_register(L, "Thing", functions);
   }

   /***************************************************************************/

   Thing *LuaThing::checkThing(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Thing **)LuaState::luaL_checkudata_ex(L, i, thingTypes);
   }

   /***************************************************************************/

   int LuaThing::getAliases(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "takes no arguments");
      }

      Thing *t = checkThing(L, -1);

      if (0 == t) {
         return luaL_error(L, "not a Thing!");
      }

      LuaArray luaAliases;
      vector<string> aliases = t->getAliases();

      for (vector<string>::const_iterator i = aliases.begin(); i != aliases.end(); i++) {

         LuaValue v;

         v.type = LUA_TYPE_STRING;
         v.value = *i;

         luaAliases.insert(luaAliases.end(), v);
      }

      LuaState::pushArray(L, luaAliases);
      return 1;
   }

   /***************************************************************************/

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

      return 0;
   }

   /***************************************************************************/

   int LuaThing::getLocation(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "takes no arguments");
      }

      Thing *t = checkThing(L, -1);

      if (0 == t) {
         return luaL_error(L, "not a Thing!");
      }

      // pushEntity will take care of case where location is null
      LuaState::pushEntity(L, t->getLocation());
      return 1;
   }

   /***************************************************************************/

   int LuaThing::setLocation(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes one argument, a Place");
      }

      Thing *t = checkThing(L, -2);
      Place *p = LuaPlace::checkPlace(L, -1);

      if (0 == t) {
         return luaL_error(L, "not a Thing!");
      }

      else if (0 == p) {
         return luaL_error(L, "not a Place!");
      }

      t->setLocation(p);
      return 0;
   }

   /***************************************************************************/

   int LuaThing::clearLocation(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "takes no arguments");
      }

      Thing *t = checkThing(L, -1);

      if (0 == t) {
         return luaL_error(L, "not a Thing!");
      }

      t->setLocation(0);
      return 0;
   }
}}

