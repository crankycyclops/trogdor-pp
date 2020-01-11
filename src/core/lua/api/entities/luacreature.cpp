#include <trogdor/game.h>
#include <trogdor/iostream/nullout.h>

#include <trogdor/entities/creature.h>
#include <trogdor/lua/api/entities/luacreature.h>

namespace trogdor { namespace entity {


   // The name of the metatable that represents the Creature metatable
   const char *LuaCreature::MetatableName = "Creature";

   // This is the name of the library that contains functions related to
   // Creatures in the game
   const char *LuaCreature::PackageName = "Creature";

   // Types which are considered valid by checkCreature()
   static const char *creatureTypes[] = {
      "Creature",
      0
   };

   // functions that take a Creature as an input (new, get, etc.)
   // format of call: Creature.new(e), where e is a Creature
   static const luaL_Reg functions[] = {
      {"new", LuaCreature::newCreature},
      {"get", LuaCreature::getCreature},
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

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Creature *LuaCreature::checkCreature(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Creature **)LuaState::luaL_checkudata_ex(L, i, creatureTypes);
   }

   /***************************************************************************/

   int LuaCreature::newCreature(lua_State *L) {

      int n = lua_gettop(L);

      if (n < 1) {
         return luaL_error(L, "creature name required");
      }

      // TODO: remove this once instantiating via a class is supported
      else if (n > 1) {
         return luaL_error(L, "creature class argument not yet supported");
      }

      std::string name = luaL_checkstring(L, -1);
      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      // Creature does not exist in the game unless it's manually inserted
      Creature *c = new Creature(nullptr, name, std::make_unique<NullOut>(), g->err().clone());

      // TODO: replace if necessary with actual class name once I support that
      c->setClass(Entity::typeToStr(entity::ENTITY_CREATURE));
      LuaState::pushEntity(L, c);

      return 1;
   }

   /***************************************************************************/

   int LuaCreature::getCreature(lua_State *L) {

      std::string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Creature *c = g->getCreature(name);

      if (c) {
         LuaState::pushEntity(L, c);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }
}}
