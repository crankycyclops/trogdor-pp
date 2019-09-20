#ifndef LUAPLAYER_H
#define LUAPLAYER_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luabeing.h"


using namespace std;

namespace trogdor { namespace entity {


   class Player;


   class LuaPlayer: public LuaBeing {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Player.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Player.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getMethods();

      public:

         /*
            Registers the Player type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Player exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Player * (or 0 if type doesn't match or doesn't exist)
         */
         static Player *checkPlayer(lua_State *L, int i);
   };
}}


#endif

