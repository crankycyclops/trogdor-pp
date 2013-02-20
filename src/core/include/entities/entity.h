#ifndef ENTITY_H
#define ENTITY_H


#include "../messages.h"


namespace core { namespace entity {


   class Game; // forward declaration for Entity

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

