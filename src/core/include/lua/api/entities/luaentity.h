#ifndef LUAENTITY_H
#define LUAENTITY_H


#include "../../luatable.h"
#include "../../luastate.h"


using namespace std;

namespace trogdor { namespace entity {


   class Entity;


   class LuaEntity {

      protected:

         /*
            Returns all functions to be registered to our Lua wrapper around
            Entity.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getFunctions();

         /*
            Returns all methods to be registered to our Lua wrapper around
            Entity.

            Input:
               (none)

            Output:
               Methods (luaL_Reg[])
         */
         static const luaL_Reg *getMethods();

      public:

         // The name of the metatable that represents the Entity metatable
         static const char *MetatableName;

         // This is the name of the library that contains functions related to
         // Entities in the game
         static const char *PackageName;

         /*
            Registers the Entity type and its associated operations in Lua.
         */
         static void registerLuaType(lua_State *L);

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
            Lua binding to Entity->getMeta(key).

            Lua input:
               Message key

            Lua output:
               Message (string)
         */
         static int getMessage(lua_State *L);

         /*
            Lua binding to Entity->setMessage(key, message).

            Lua input:
               Message key
               Message value

            Lua output:
               (none)
         */
         static int setMessage(lua_State *L);

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
            Lua binding to Entity->setTitle().

            Lua input:
               New title (string)

            Lua output:
               (none)
         */
         static int setTitle(lua_State *L);

         /*
            Lua binding to Entity->getLongDescription().

            Lua input:
               (none)

            Lua output:
               a string containing the Entity's long description
         */
         static int getLongDesc(lua_State *L);

         /*
            Lua binding to Entity->setLongDescription().

            Lua input:
               New description (string)

            Lua output:
               (none)
         */
         static int setLongDesc(lua_State *L);

         /*
            Lua binding to Entity->getShortDescription().

            Lua input:
               (none)

            Lua output:
               a string containing the Entity's short description
         */
         static int getShortDesc(lua_State *L);

         /*
            Lua binding to Entity->setShortDescription().

            Lua input:
               New description (string)

            Lua output:
               (none)
         */
         static int setShortDesc(lua_State *L);

         /*
            Lua binding to Entity->observe().

            Lua input:
               Being observing the Entity (required)
               Whether or not to trigger events (default: true)
               Whether or not to always display full descripton (default: false)

            Lua output:
               (none)
         */
         static int glance(lua_State *L);

         /*
            Lua binding to Entity->glance().

            Lua input:
               Being observing the Entity (required)
               Whether or not to trigger events (default: true)

            Lua output:
               (none)
         */
         static int observe(lua_State *L);

         /*
            Lua binding to Entity->glancedBy().

            Lua input:
               Being (Being *) that may or may not have glanced at the Entity.

            Lua output:
               (none)
         */
         static int glancedBy(lua_State *L);

         /*
            Lua binding to Entity->observedBy().

            Lua input:
               Being (Being *) that may or may not have observed the Entity.

            Lua output:
               (none)
         */
         static int observedBy(lua_State *L);
   };
}}


#endif

