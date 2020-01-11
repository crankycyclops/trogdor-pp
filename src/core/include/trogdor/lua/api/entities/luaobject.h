#ifndef LUAOBJECT_H
#define LUAOBJECT_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/lua/api/entities/luathing.h>


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

         /*
            Returns a new Object identified by the given name.

            Lua input:
               Object name (string)
               TODO: add support for optional second class name argument
                  (requires classes to be stored outside the instantiator.)

            Lua output:
               Instance of Object or nil on error
         */
         static int newObject(lua_State *L);

         /*
            Gets the Object from the global game object referenced by the given
            name. If the Object doesn't exist, returns nil.

            Lua input:
               Object's name (string)

            Lua output:
               Object or nil if it doesn't exist
         */
         static int getObject(lua_State *L);
   };
}}


#endif
