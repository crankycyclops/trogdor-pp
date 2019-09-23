#ifndef LUAPLACE_H
#define LUAPLACE_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luaentity.h"


using namespace std;

namespace trogdor { namespace entity {


   class Place;


   class LuaPlace: public LuaEntity {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Place.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Place.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Place metatable
         static const char *MetatableName;

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

         /*
            Wraps around Place::insertThing().

            Input:
               Thing to insert

            Output:
               (none)
         */
         static int insertThing(lua_State *L);

         /*
            Wraps around Place::removeThing().

            Input:
               Thing to insert

            Output:
               (none)
         */
         static int removeThing(lua_State *L);
   };
}}


#endif

