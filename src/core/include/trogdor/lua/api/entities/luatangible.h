#ifndef LUATANGIBLE_H
#define LUATANGIBLE_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/lua/api/entities/luaentity.h>


namespace trogdor::entity {


   class Tangible;


   class LuaTangible: public LuaEntity {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Tangible.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Tangible.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Tangible metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Tangibles in the game
         static const char *PackageName;

         /*
            Registers the Tangible type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Tangible exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Tangible * (or 0 if type doesn't match or doesn't exist)
         */
         static Tangible *checkTangible(lua_State *L, int i);

         /*
            Gets the Tangible from the global game object referenced by the
            given name. If the Tangible doesn't exist, returns nil.

            Lua input:
               Tangible's name (string)

            Lua output:
               Tangible or nil if it doesn't exist
         */
         static int getTangible(lua_State *L);

         /*
            Lua binding to Tangible->observe().

            Lua input:
               Being observing the Tangible (required)
               Whether or not to trigger events (default: true)
               Whether or not to always display full descripton (default: false)

            Lua output:
               (none)
         */
         static int glance(lua_State *L);

         /*
            Lua binding to Tangible->glance().

            Lua input:
               Being observing the Tangible (required)
               Whether or not to trigger events (default: true)

            Lua output:
               (none)
         */
         static int observe(lua_State *L);

         /*
            Lua binding to Tangible->glancedBy().

            Lua input:
               Being (Being *) that may or may not have glanced at the Tangible.

            Lua output:
               (none)
         */
         static int glancedBy(lua_State *L);

         /*
            Lua binding to Tangible->observedBy().

            Lua input:
               Being (Being *) that may or may not have observed the Tangible.

            Lua output:
               (none)
         */
         static int observedBy(lua_State *L);
   };
}


#endif
