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

         static void registerLuaType(lua_State *L);

         /*
            Lua binding to Entity->getType().
         */
         static int getType(lua_State *L);
   };
}}


#endif

