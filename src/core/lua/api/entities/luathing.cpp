#include <vector>

#include <trogdor/game.h>

#include <trogdor/entities/thing.h>
#include <trogdor/entities/place.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/being.h>

#include <trogdor/lua/api/entities/luathing.h>
#include <trogdor/lua/api/entities/luaplace.h>

#include <trogdor/exception/exception.h>

namespace trogdor::entity {


   // The name of the metatable that represents the Thing metatable
   const char *LuaThing::MetatableName = "Thing";

   // This is the name of the library that contains functions related to Things
   // in the game
   const char *LuaThing::PackageName = "Thing";

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
   static const luaL_Reg functions[] = {
      {"get", LuaThing::getThing},
      {0, 0}
   };

   // Lua Thing methods that bind to C++ Thing methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {"getAliases",    LuaThing::getAliases},
      {"addAlias",      LuaThing::addAlias},
      {"getLocation",   LuaThing::getLocation},
      {"setLocation",   LuaThing::setLocation},
      {"clearLocation", LuaThing::removeFromLocation},
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaThing::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaThing::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaThing::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Thing.__index = Thing
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Thing *LuaThing::checkThing(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Thing **)LuaState::luaL_checkudata_ex(L, i, thingTypes);
   }

   /***************************************************************************/

   int LuaThing::getThing(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Thing *t = g->getThing(name).get();

      if (t) {
         LuaState::pushEntity(L, t);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }

   /***************************************************************************/

   int LuaThing::getAliases(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "takes no arguments");
      }

      Thing *t = checkThing(L, -1);

      if (nullptr == t) {
         return luaL_error(L, "not a Thing!");
      }

      LuaArray luaAliases;
      std::vector<std::string> aliases = t->getAliases();

      for (auto &alias: aliases) {

         LuaValue v;

         v.type = LUA_TYPE_STRING;
         v.value = alias;

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

      Thing *t = checkThing(L, -n);

      if (nullptr == t) {
         return luaL_error(L, "not a Thing!");
      }

      try {

         std::string alias = luaL_checkstring(L, -1);
         t->addAlias(alias);

         return 0;
      }

      catch (const Exception &e) {
         return luaL_error(L, e.what());
      }
   }

   /***************************************************************************/

   int LuaThing::getLocation(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "takes no arguments");
      }

      Thing *t = checkThing(L, -1);

      if (nullptr == t) {
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
         return luaL_error(L, "takes one Place argument (use colon for method call)");
      }

      Thing *t = checkThing(L, -2);
      Place *p = LuaPlace::checkPlace(L, -1);

      if (nullptr == t) {
         return luaL_error(L, "calling object is not a Thing");
      }

      else if (nullptr == p) {
         return luaL_error(L, "argument is not a Place");
      }

      try {

         // If the Thing is an Object and has an owner, then we need to remove
         // it from the owner's inventory first
         if (ENTITY_OBJECT == t->getType()) {

            Object *objRef = dynamic_cast<Object *>(t);
            Being *owner = objRef->getOwner();

            if (owner) {
               owner->removeFromInventory(objRef->getShared());
            }
         }

         t->setLocation(p);
         return 0;
      }

      catch (const Exception &e) {
         return luaL_error(L, e.what());
      }
   }

   /***************************************************************************/

   int LuaThing::removeFromLocation(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "takes no arguments");
      }

      Thing *t = checkThing(L, -1);

      if (nullptr == t) {
         return luaL_error(L, "not a Thing!");
      }

      try {

         Place *p = t->getLocation();

         if (p) {
            p->removeThing(t);
         }

         return 0;
      }

      catch (const Exception &e) {
         return luaL_error(L, e.what());
      }
   }
}
