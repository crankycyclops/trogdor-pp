#include "../../../include/entities/object.h"
#include "../../../include/lua/api/entities/luaobject.h"

using namespace std;

namespace core { namespace entity {


   // Types which are considered valid by checkObject()
   static const char *objectTypes[] = {
      "Object",
      0
   };

   // functions that take an Object as an input (new, get, etc.)
   // format of call: Object.new(e), where e is an Object
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Object methods that bind to C++ Object methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };


   const luaL_reg *LuaObject::getFunctions() {

      return functions;
   }

   const luaL_reg *LuaObject::getMethods() {

      return methods;
   }

   void LuaObject::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Object");

      // Object.__index = Object
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, LuaThing::getMethods());
      luaL_register(L, 0, LuaItem::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Object", LuaEntity::getFunctions());
      luaL_register(L, "Object", LuaThing::getFunctions());
      luaL_register(L, "Object", LuaItem::getFunctions());
      luaL_register(L, "Object", functions);
   }

   Object *LuaObject::checkObject(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Object **)LuaState::luaL_checkudata_ex(L, i, objectTypes);
   }
}}
