#include "../../../include/game.h"
#include "../../../include/entities/being.h"
#include "../../../include/lua/api/entities/luabeing.h"

using namespace std;

namespace trogdor { namespace entity {


   // The name of the metatable that represents the Being metatable
   const char *LuaBeing::MetatableName = "Being";

   // This is the name of the library that contains functions related to Beings
   // in the game
   const char *LuaBeing::PackageName = "Being";

   // Types which are considered valid by checkBeing()
   static const char *beingTypes[] = {
      "Being",
      "Player",
      "Creature",
      0
   };

   // functions that take a Being as an input (new, get, etc.)
   // format of call: Being.new(e), where e is a Being
   static const luaL_Reg functions[] = {
      {"get", LuaBeing::getBeing},
      {0, 0}
   };

   // Lua Being methods that bind to C++ Being methods
   // also includes meta methods
   static const luaL_Reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaBeing::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaBeing::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaBeing::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Being.__index = Being
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, LuaEntity::getMethods());
      LuaState::luaL_register_wrapper(L, 0, LuaThing::getMethods());
      LuaState::luaL_register_wrapper(L, 0, methods);

      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Being *LuaBeing::checkBeing(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Being **)LuaState::luaL_checkudata_ex(L, i, beingTypes);
   }

   /***************************************************************************/

   int LuaBeing::getBeing(lua_State *L) {

      string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Being *b = g->getBeing(name);

      if (b) {
         LuaState::pushEntity(L, b);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }
}}

