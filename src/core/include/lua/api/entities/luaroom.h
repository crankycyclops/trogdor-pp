#ifndef LUAROOM_H
#define LUAROOM_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luaplace.h"


using namespace std;

namespace trogdor { namespace core { namespace entity {


   class Room;


   class LuaRoom: public LuaPlace {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Room.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Room.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getMethods();

      public:

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
   };
}}}


#endif

