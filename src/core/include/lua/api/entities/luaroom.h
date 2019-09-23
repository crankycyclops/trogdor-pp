#ifndef LUAROOM_H
#define LUAROOM_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luaplace.h"


using namespace std;

namespace trogdor { namespace entity {


   class Room;


   class LuaRoom: public LuaPlace {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Room.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Room.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Room metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Rooms in the game
         static const char *PackageName;

         /*
            Registers the Room type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Room exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Room * (or 0 if type doesn't match or doesn't exist)
         */
         static Room *checkRoom(lua_State *L, int i);

         /*
            Gets the Room from the global game object referenced by the given
            name. If the Room doesn't exist, returns nil.

            Lua input:
               Room's name (string)

            Lua output:
               Room or nil if it doesn't exist
         */
         static int getRoom(lua_State *L);
   };
}}


#endif

