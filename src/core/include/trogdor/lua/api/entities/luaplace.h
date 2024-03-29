#ifndef LUAPLACE_H
#define LUAPLACE_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/lua/api/entities/luatangible.h>


namespace trogdor::entity {


   class Place;


   class LuaPlace: public LuaTangible {


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

         // This is the name of the library that contains functions related to
         // Places in the game
         static const char *PackageName;

         // Types which are considered valid by checkPlace()
         static const char *types[];

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
            Gets the Place from the global game object referenced by the given
            name. If the Place doesn't exist, returns nil.

            Lua input:
               Place's name (string)

            Lua output:
               Place or nil if it doesn't exist
         */
         static int getPlace(lua_State *L);

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
}


#endif
