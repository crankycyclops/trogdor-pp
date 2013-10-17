#ifndef LUAPLACE_H
#define LUAPLACE_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luaentity.h"


using namespace std;

namespace core { namespace entity {


   class Place;


   class LuaPlace: public LuaEntity {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Place.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Place.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getMethods();

      public:

         /*
            Registers the Place type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Place exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Place * (or 0 if type doesn't match or doesn't exist)
         */
         static Place *checkPlace(lua_State *L, int i);
   };
}}


#endif

