#ifndef ENTITY_H
#define ENTITY_H


#include "../messages.h"
#include "../luastate.h"

namespace core {

   class Game;
   class Parser;

   namespace event {

      class EventListener;
   }
}

using namespace core::event;


namespace core { namespace entity {


   // used for run-time check of an Entity's type
   enum EntityType {
      ENTITY_UNDEFINED,
      ENTITY_ROOM,
      ENTITY_PLAYER,
      ENTITY_CREATURE,
      ENTITY_OBJECT
   };

   class Entity {

      protected:

         enum EntityType type;

         Game *game;

         string name;
         string title;
         string longDesc;
         string shortDesc;

         Messages msgs;

         LuaState *L;
         EventListener *triggers;

      public:

         // This is annoying, but due to unforseen design problems, this is now
         // my only option (grrr...)
         friend class core::Parser;

         /*
            Constructor for creating a new Entity.  Requires reference to the
            containing Game object and a name.

            Input:
               Reference to containing Game (Game *)
               Name (string)
         */
         Entity(Game *g, string n);

         /*
            Returns the Entity's type.

            Input:
               (none)

            Output:
               (none)
         */
         inline enum EntityType getType() const {return type;}

         /*
            Returns the Entity's name.

            Input:
               (none)

            Output:
               Entity's name (string)
         */
         inline string getName() const {return name;}

         /*
            Returns the Entity's title.

            Input:
               (none)

            Output:
               Entity's title (string)
         */
         inline string getTitle() const {return title;}

         /*
            Returns the Entity's long description.

            Input:
               (none)

            Output:
               Entity's long description (string)
         */
         inline string getLongDescription() const {return longDesc;}

         /*
            Returns the Entity's short description.

            Input:
               (none)

            Output:
               Entity's short description (string)
         */
         inline string getShortDescription() const {return shortDesc;}

         /*
            Copies another Messages object into our own, replacing all previous
            content.  This should only ever be used by the Parser.

            Input:
               Reference to Messages object

            Output:
               (none)
         */
         inline void setMessages(Messages &m) {msgs = m;}

         /*
            Sets the Entity's title.

            Input:
               New title (string)

            Output:
               (none)
         */
         inline void setTitle(string t) {title = t;}

         /*
            Sets the Entity's long description.

            Input:
               New long description (string)

            Output:
               (none)
         */
         inline void setLongDescription(string d) {longDesc = d;}

         /*
            Sets the Entity's short description.

            Input:
               New description (string)

            Output:
               (none)
         */
         inline string setShortDescription(string d) {shortDesc = d;}
   };
}}


#endif

