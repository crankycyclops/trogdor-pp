#ifndef ENTITY_H
#define ENTITY_H


#include "../messages.h"
#include "../luastate.h"

namespace core {

   class Game;

   namespace event {

      class EventListener;
   }
}

using namespace core::event;


namespace core { namespace entity {


   class Entity {

      protected:

         Game *game;

         string name;
         string title;
         string longDesc;
         string shortDesc;

         Messages msgs;

         LuaState *L;
         EventListener *triggers;

      public:

         /*
            Constructor for creating a new Entity.  Requires reference to the
            containing Game object and a name.
         */
         Entity(Game *g, string n);
   };
}}


#endif

