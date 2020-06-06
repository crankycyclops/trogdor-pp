#include <trogdor/game.h>

#include <trogdor/entities/place.h>
#include <trogdor/entities/thing.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/being.h>

#include <trogdor/lua/api/entities/luaplace.h>
#include <trogdor/lua/api/entities/luathing.h>

namespace trogdor::entity {


   // The name of the metatable that represents the Place metatable
   const char *LuaPlace::MetatableName = "Place";

   // This is the name of the library that contains functions related to Places
   // in the game
   const char *LuaPlace::PackageName = "Place";

   // Types which are considered valid by checkPlace()
   static const char *placeTypes[] = {
      "Place",
      "Room",
      0
   };

   // functions that take a Place as an input (new, get, etc.)
   // format of call: Place.new(e), where e is a Place
   static const luaL_Reg functions[] = {
      {"get", LuaPlace::getPlace},
      {0, 0}
   };

   // Lua Place methods that bind to C++ Place methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {"insertThing", LuaPlace::insertThing},
      {"removeThing", LuaPlace::removeThing},
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaPlace::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaPlace::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaPlace::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Place.__index = Place
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Place *LuaPlace::checkPlace(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Place **)LuaState::luaL_checkudata_ex(L, i, placeTypes);
   }

   /***************************************************************************/

   int LuaPlace::getPlace(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Place *p = g->getPlace(name).get();

      if (p) {
         LuaState::pushEntity(L, p);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }

   /***************************************************************************/

   int LuaPlace::insertThing(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes one argument, a Place");
      }

      Place *p = checkPlace(L, -2);
      Thing *t = LuaThing::checkThing(L, -1);

      if (nullptr == t) {
         return luaL_error(L, "not a Thing!");
      }

      else if (nullptr == p) {
         return luaL_error(L, "not a Place!");
      }

      if (auto oldLocation = t->getLocation().lock()) {
         oldLocation->removeThing(t->getShared());
      }

      if (ENTITY_OBJECT == t->getType()) {

         std::shared_ptr<Object> objShared = static_cast<Object *>(t)->getShared();

         if (std::shared_ptr<Being> owner = objShared->getOwner().lock()) {
            owner->removeFromInventory(objShared);
         }
      }

      p->insertThing(t->getShared());
      return 0;
   }

   /***************************************************************************/

   int LuaPlace::removeThing(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes one argument, a Place");
      }

      Place *p = checkPlace(L, -2);
      Thing *t = LuaThing::checkThing(L, -1);

      if (nullptr == t) {
         return luaL_error(L, "not a Thing!");
      }

      else if (nullptr == p) {
         return luaL_error(L, "not a Place!");
      }

      p->removeThing(t->getShared());
      return 0;
   }
}
