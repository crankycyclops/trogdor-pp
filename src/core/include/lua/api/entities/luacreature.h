#ifndef LUACREATURE_H
#define LUACREATURE_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luabeing.h"


using namespace std;

namespace trogdor { namespace entity {


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
   };
}}


#endif

