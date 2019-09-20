#ifndef LUABEING_H
#define LUABEING_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luathing.h"


using namespace std;

namespace trogdor { namespace entity {


   class Being;


   class LuaBeing: public LuaThing {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Being.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Being.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getMethods();

      public:

         /*
            Registers the Being type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Being exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Being * (or 0 if type doesn't match or doesn't exist)
         */
         static Being *checkBeing(lua_State *L, int i);
   };
}}


#endif

