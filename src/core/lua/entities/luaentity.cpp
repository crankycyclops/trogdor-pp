#include "../../include/entities/entity.h"
#include "../../include/lua/entities/luaentity.h"

using namespace std;

namespace core { namespace entity {


   // Lua Entity methods that bind to C++ Entity methods
   // also includes meta methods
   static const luaL_reg methodsEntity[] = {
      {"out",        LuaEntity::out},
      {"getType",    LuaEntity::getType},
      {"getName",    LuaEntity::getName},
      {"__tostring", LuaEntity::getName},
      {0, 0}
   };

   // functions that take an Entity as an input (new, get, etc.)
   // format of call: Entity.new(e), where e is an Entity
   static const luaL_reg functionsEntity[] = {
      {0, 0}
   };

   void LuaEntity::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Entity");

      // Entity.__index = Entity
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, methodsEntity);
      luaL_register(L, "Entity", functionsEntity);
   }


   Entity *LuaEntity::checkEntity(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Entity **)luaL_checkudata(L, i, "Entity");
   }


   int LuaEntity::out(lua_State *L) {

      const char *message;
      const char *channel;

      int n = lua_gettop(L);

      if (n > 3) {
         return luaL_error(L, "too many arguments");
      }

      if (n < 2) {
         message = "\n";
      } else {
         message = lua_tostring(L, 1 - n);
      }

      if (n < 3) {
         channel = "notifications";
      } else {
         channel = lua_tostring(L, -1);
      }

      Entity *e = checkEntity(L, -n);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      e->out(channel) << message;
      e->out(channel).flush();

      return 1;
   }


   int LuaEntity::getType(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "getType takes no arguments");
      }

      Entity *e = checkEntity(L, 1);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getTypeName().c_str());
      return 1;
   }


   int LuaEntity::getName(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "getName takes no arguments");
      }

      Entity *e = checkEntity(L, 1);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getName().c_str());
      return 1;
   }
}}

