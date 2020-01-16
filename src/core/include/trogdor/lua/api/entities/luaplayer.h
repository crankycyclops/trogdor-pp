#ifndef LUAPLAYER_H
#define LUAPLAYER_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/lua/api/entities/luabeing.h>


namespace trogdor::entity {


   class Player;


   class LuaPlayer: public LuaBeing {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Player.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Player.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Player metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Players in the game
         static const char *PackageName;

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

         /*
            Gets the Player from the global game object referenced by the given
            name. If the Player doesn't exist, returns nil.

            Lua input:
               Player's name (string)

            Lua output:
               Player or nil if it doesn't exist
         */
         static int getPlayer(lua_State *L);
   };
}


#endif
