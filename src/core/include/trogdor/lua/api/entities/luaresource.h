#ifndef LUARESOURCE_H
#define LUARESOURCE_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/lua/api/entities/luaentity.h>


namespace trogdor::entity {


   class Resource;


   class LuaResource: public LuaEntity {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Resource.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Resource.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Resource metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Resources in the game
         static const char *PackageName;

         /*
            Registers the Resource type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Resource exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Resource * (or 0 if type doesn't match or doesn't exist)
         */
         static Resource *checkResource(lua_State *L, int i);

         /*
            Gets the Resource from the global game object referenced by the
            given name. If the Resource doesn't exist, returns nil.

            Lua input:
               Resource's name (string)

            Lua output:
               Resource or nil if it doesn't exist
         */
         static int getResource(lua_State *L);
   };
}


#endif
