#ifndef ENTITY_H
#define ENTITY_H


#include "../game.h"
#include "../messages.h"


namespace core { namespace entity {

   class Entity {

      protected:

         Game *game;

         string name;
         string title;
         string longDesc;
         string shortDesc;

         Messages msgs;

         //TODO: lua_State *L;
         // TODO: event handlers

      public:

   };
}}


#endif

