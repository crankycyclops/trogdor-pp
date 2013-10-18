#include "../../../include/entities/item.h"
#include "../../../include/lua/api/entities/luaitem.h"

using namespace std;

namespace core { namespace entity {


   // Types which are considered valid by checkItem()
   static const char *itemTypes[] = {
      "Item",
      "Object",
      0
   };

   // functions that take an Item as an input (new, get, etc.)
   // format of call: Item.new(e), where e is an Item
   static const luaL_reg functions[] = {
      {0, 0}
   };

   // Lua Item methods that bind to C++ Item methods
   // also includes meta methods
   static const luaL_reg methods[] = {
      {0, 0}
   };

   /***************************************************************************/

   const luaL_reg *LuaItem::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_reg *LuaItem::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaItem::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, "Item");

      // Item.__index = Item
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, LuaEntity::getMethods());
      luaL_register(L, 0, LuaThing::getMethods());
      luaL_register(L, 0, methods);
      luaL_register(L, "Item", LuaEntity::getFunctions());
      luaL_register(L, "Item", LuaThing::getFunctions());
      luaL_register(L, "Item", functions);
   }

   /***************************************************************************/

   Item *LuaItem::checkItem(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Item **)LuaState::luaL_checkudata_ex(L, i, itemTypes);
   }
}}

