#include <trogdor/game.h>
#include <trogdor/iostream/nullout.h>

#include <trogdor/entities/object.h>
#include <trogdor/lua/api/entities/luaobject.h>

namespace trogdor::entity {


   // The name of the metatable that represents the Object metatable
   const char *LuaObject::MetatableName = "Object";

   // This is the name of the library that contains functions related to
   // Objects in the game
   const char *LuaObject::PackageName = "Object";

   // Types which are considered valid by checkObject()
   const char *LuaObject::types[] = {
      "Object",
      0
   };

   // functions that take an Object as an input (new, get, etc.)
   // format of call: Object.new(e), where e is an Object
   static const luaL_Reg functions[] = {
      {"new",   LuaObject::newObject},
      {"clone", LuaObject::cloneObject},
      {"get",   LuaObject::getObject},
      {0, 0}
   };

   // Lua Object methods that bind to C++ Object methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaObject::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaObject::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaObject::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Object.__index = Object
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaThing::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Object *LuaObject::checkObject(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Object **)LuaState::luaL_checkudata_ex(L, i, types);
   }

   /***************************************************************************/

   int LuaObject::newObject(lua_State *L) {

      int n = lua_gettop(L);

      if (n < 1) {
         return luaL_error(L, "object name required");
      }

      // TODO: remove this once instantiating via a class is supported
      else if (n > 1) {
         return luaL_error(L, "object class argument not yet supported");
      }

      std::string name = luaL_checkstring(L, -1);
      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      // Object will not exist in the game unless it's manually inserted
      Object *o = new Object(nullptr, name, std::make_unique<NullOut>(), g->err().copy());
      o->setManagedByLua(true);

      // TODO: replace with class name once I support that
      o->setClass(Entity::typeToStr(entity::ENTITY_OBJECT));
      LuaState::pushEntity(L, o);

      return 1;
   }

   /***************************************************************************/

   int LuaObject::cloneObject(lua_State *L) {

      int n = lua_gettop(L);

      if (n < 2) {
         return luaL_error(L, "prototype instance of Object and new name required");
      }

      Object *prototype = checkObject(L, -2);
      std::string name = luaL_checkstring(L, -1);

      // Object will not exist in the game unless it's manually inserted
      Object *o = new Object(*prototype, name);
      o->setManagedByLua(true);

      LuaState::pushEntity(L, o);
      return 1;
   }

   /***************************************************************************/

   int LuaObject::getObject(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Object *o = g->getObject(name).get();

      if (o) {
         LuaState::pushEntity(L, o);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }
}
