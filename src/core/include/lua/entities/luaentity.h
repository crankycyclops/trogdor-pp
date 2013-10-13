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
            Wraps around an Entity's input stream to read a string, and returns
            that string.

            Lua input:
               (none)

            Lua output:
               string read from the Entity's input stream
         */
         static int in(lua_State *L);

         /*
            Pushes a string to the Entity's output stream at the specified
            channel (defaults to "notifications.") If no string is passed, a
            newline is printed to the default channel.

            Lua input:
               Message to push to Entity's output stream
               Channel the message should be routed to (optional: defaults to
                  "notifications")

            Lua output:
               (none)
         */
         static int out(lua_State *L);

         /*
            Lua binding to Entity->getMeta(key).

            Lua input:
               Meta value key

            Lua output:
               Meta value (string)
         */
         static int getMeta(lua_State *L);

         /*
            Lua binding to Entity->setMeta(key, value).

            Lua input:
               Meta value key
               Meta value

            Lua output:
               (none)
         */
         static int setMeta(lua_State *L);

         /*
            Takes as input a string Entity type, maps it to an Entity type and
            calls Entity->isType(). Returns true if Entity is of that type and
            false if it's not.

            Lua input:
               string representation of an Entity type

            Lua output:
               true if Entity is of that type and false if it's not
         */
         static int isType(lua_State *L);

         /*
            Lua binding to Entity->getType().

            Lua input:
               (none)

            Lua output:
               a string describing the Entity's type
         */
         static int getType(lua_State *L);

         /*
            Lua binding to Entity->getName().

            Lua input:
               (none)

            Lua output:
               a string containing the Entity's name
         */
         static int getName(lua_State *L);

         /*
            Lua binding to Entity->getTitle().

            Lua input:
               (none)

            Lua output:
               a string containing the Entity's title
         */
         static int getTitle(lua_State *L);

         /*
            Lua binding to Entity->getLongDescription().

            Lua input:
               (none)

            Lua output:
               a string containing the Entity's long description
         */
         static int getLongDesc(lua_State *L);

         /*
            Lua binding to Entity->getShortDescription().

            Lua input:
               (none)

            Lua output:
               a string containing the Entity's short description
         */
         static int getShortDesc(lua_State *L);
   };
}}


#endif

