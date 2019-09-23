#ifndef LUAOBJECT_H
#define LUAOBJECT_H


#include "../../luatable.h"
#include "../../luastate.h"

#include "luathing.h"


using namespace std;

namespace trogdor { namespace entity {


   class Object;


   class LuaObject: public LuaThing {


      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Object.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Object.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Object metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Objects in the game
         static const char *PackageName;

         /*
            Registers the Object type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Checks that an Object exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Object * (or 0 if type doesn't match or doesn't exist)
         */
         static Object *checkObject(lua_State *L, int i);
   };
}}


#endif

