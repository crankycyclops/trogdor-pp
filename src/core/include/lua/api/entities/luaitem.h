#ifndef LUAITEM_H
#define LUAITEM_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luathing.h"


using namespace std;

namespace core { namespace entity {


   class Item;


   class LuaItem: public LuaThing {


      protected:

         /*
            Checks that an Item exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Item * (or 0 if type doesn't match or doesn't exist)
         */
         static Item *checkItem(lua_State *L, int i);

         /*
            Returns all functions to be registered to our Lua wrapper around
            Item.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Item.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getMethods();

      public:

         /*
            Registers the Item type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);
   };
}}


#endif

