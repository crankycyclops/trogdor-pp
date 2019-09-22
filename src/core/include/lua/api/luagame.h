#ifndef LUAGAME_H
#define LUAGAME_H


#include "../luatable.h"
#include "../luastate.h"


using namespace std;

namespace trogdor {


   class LuaGame {

      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Game.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Game.

            Input:
               (none)

            Output:
               Methods (luaL_reg[])
         */
         static const luaL_reg *getMethods();

      public:

         /*
            Registers the Entity type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);
   };
}


#endif

