#ifndef LUAENTITY_H
#define LUAENTITY_H


#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>


namespace trogdor::entity {


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

         // Types that are considered valid by checkEntity()
         static const char *types[];

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
            This method is called by the garbage collector to delete an Entity
            allocated by a Lua call. This method does nothing if the Entity
            belongs to a Game (in which case it's a smart pointer and managed
            outside the scope of Lua.)

            Lua input:
               The entity to be deleted (Entity)

            Lua output:
               (none)
         */
         static int gcEntity(lua_State *L);

         /*
            Gets the Entity from the global game object referenced by the given
            name. If the Entity doesn't exist, returns nil.

            Lua input:
               Entity's name (string)

            Lua output:
               Entity or nil if it doesn't exist
         */
         static int getEntity(lua_State *L);

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
            Pushes a string to the Entity's error stream at the specified
            error level (defaults to ERROR.)

            Lua input:
               Message to push to Entity's error stream
               Optional severity level (integer 0 = INFO, 1 = WARNING, and 2 = ERROR. Any other integers result in undefined behavior.)

            Lua output:
               (none)
         */
         static int err(lua_State *L);

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
            Lua binding to Entity->setTag(tagName).

            Lua input:
               Tag name to set

            Lua output:
               (none)
         */
         static int setTag(lua_State *L);

         /*
            Lua binding to Entity->removeTag(tagName).

            Lua input:
               Tag name to remove

            Lua output:
               (none)
         */
         static int removeTag(lua_State *L);

         /*
            Lua binding to Entity->isTagSet(tagName).

            Lua input:
               Tag name

            Lua output:
               true if the tag is set and false if it's not
         */
         static int isTagSet(lua_State *L);

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
   };
}


#endif
