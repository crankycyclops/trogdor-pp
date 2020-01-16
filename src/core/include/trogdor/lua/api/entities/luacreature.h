#ifndef LUACREATURE_H
#define LUACREATURE_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/lua/api/entities/luabeing.h>


namespace trogdor::entity {


   class Creature;


   class LuaCreature: public LuaBeing {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Creature.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Creature.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Creature metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Creatures in the game
         static const char *PackageName;

         /*
            Registers the Creature type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that a Creature exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Creature * (or 0 if type doesn't match or doesn't exist)
         */
         static Creature *checkCreature(lua_State *L, int i);

         /*
            Returns a new Creature identified by the given name.

            Lua input:
               Creature name (string)
               TODO: add support for optional second class name argument
                  (requires classes to be stored outside the instantiator.)

            Lua output:
               Instance of Creature or nil on error
         */
         static int newCreature(lua_State *L);

         /*
            Gets the Creature from the global game object referenced by the given
            name. If the Creature doesn't exist, returns nil.

            Lua input:
               Creature's name (string)

            Lua output:
               Creature or nil if it doesn't exist
         */
         static int getCreature(lua_State *L);
   };
}


#endif
