#ifndef LUABEING_H
#define LUABEING_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/lua/api/entities/luathing.h>


namespace trogdor::entity {


   class Being;


   class LuaBeing: public LuaThing {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Being.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Being.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Being metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Beings in the game
         static const char *PackageName;

         /*
            Registers the Being type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Being exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Being * (or 0 if type doesn't match or doesn't exist)
         */
         static Being *checkBeing(lua_State *L, int i);

         /*
            Gets the Being from the global game object referenced by the given
            name. If the Being doesn't exist, returns nil.

            Lua input:
               Being's name (std::string)

            Lua output:
               Being or nil if it doesn't exist
         */
         static int getBeing(lua_State *L);

         /*
            Wraps around Being::insertIntoInventory().

            Input:
               Object to insert
               Whether or not weight should be considered

            Output:
               (none)
         */
         static int insertIntoInventory(lua_State *L);

         /*
            Wraps around Being::removeFromInventory().

            Input:
               Object to remove

            Output:
               (none)
         */
         static int removeFromInventory(lua_State *L);
   };
}


#endif
