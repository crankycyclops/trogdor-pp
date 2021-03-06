#ifndef LUAGAME_H
#define LUAGAME_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>


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

         // This is the name of the library that contains functions related to
         // the Lua Game object
         static const char *PackageName;

         // This is the variable name of the global instance of Game
         static const char *globalName;

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
            Inserts an Entity created by a Lua script into the game. If an
            Entity OF ANY TYPE already exists in the game, this function will
            return boolean false to indicate failure. Otherwise, it will return
            boolean true to indicate success.

            Lua input:
               Entity to insert (Entity)

            Lua output:
               Boolean true if the operation is successful and false if it's not
         */
         static int insertEntity(lua_State *L);

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
