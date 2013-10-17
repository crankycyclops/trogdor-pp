#ifndef LUATHING_H
#define LUATHING_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luaentity.h"


using namespace std;

namespace core { namespace entity {


   class Thing;


   class LuaThing: public LuaEntity {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Thing.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Thing.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getMethods();

      public:

         /*
            Registers the Thing type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Thing exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Thing * (or 0 if type doesn't match or doesn't exist)
         */
         static Thing *checkThing(lua_State *L, int i);

         /*
            Wraps around Thing::getAliases.

            Input:
               (none)

            Output:
               Array of aliases (string[])
         */
         static int getAliases(lua_State *L);

         /*
            Wraps around Thing::addAlias, and allows a script to add an alias to
            a Thing.

            Input:
               New alias (string)

            Output:
               (none)
         */
         static int addAlias(lua_State *L);
   };
}}


#endif

