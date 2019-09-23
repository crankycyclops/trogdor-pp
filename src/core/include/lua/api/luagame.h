#ifndef LUAGAME_H
#define LUAGAME_H


#include "../luatable.h"
#include "../luastate.h"


using namespace std;

namespace trogdor {


   // Forward declaration of Game
   class Game;


   class LuaGame {

      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Game.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Game.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents our global Lua Game object
         static const char *MetatableName;

         /*
            Registers the Entity type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a pointer to a Game object exists at the specified
            location on the Lua stack and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Game * (or 0 if type doesn't match or doesn't exist)
         */
         static Game *checkGame(lua_State *L, int i);

         /*
            Lua binding to Game->getTiem().

            Lua input:
               (none)

            Lua output:
               Current game time (number)
         */
         static int getTime(lua_State *L);
   };
}


#endif

