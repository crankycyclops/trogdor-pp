#ifndef LUAENTITY_H
#define LUAENTITY_H


#include "../luatable.h"
#include "../luastate.h"


using namespace std;

namespace core { namespace entity {


   class Entity;


   class LuaEntity {


      protected:

         /*
            Checks that an Entity exists at the specified location on the Lua
            stack, and returns it if it does.

            Input:
               Lua State
               Index on stack

            Output:
               Entity * (or 0 if type doesn't match or doesn't exist)
         */
         static Entity *checkEntity(lua_State *L, int i);

      public:

         /*
            Registers the Entity type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

         /*
            Pushes a string to the Entity's output stream at the specified
            channel (defaults to "notifications.") If no string is passed, a
            newline is printed to the default channel.

            Lua input:
               Message to push to Entity's output stream
               Channel the message should be routed to (optional: defaults to
                  "notifications")

            Lua output:
               a string describing the Entity's type
         */
         static int out(lua_State *L);

         /*
            Lua binding to Entity->getType().

            Lua input:
               (none)

            Lua output:
               a string describing the Entity's type
         */
         static int getType(lua_State *L);
   };
}}


#endif

