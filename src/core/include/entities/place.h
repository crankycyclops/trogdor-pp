#ifndef PLACE_H
#define PLACE_H


#include <list>
#include <cstring>
#include <sstream>
#include <boost/unordered_map.hpp>
#include <boost/type_traits.hpp>

#include "entity.h"

using namespace boost;


namespace core { namespace entity {

   // Forward declarations to prevent circular dependency issues
   class Thing;
   class Being;
   class Item;
   class Player;
   class Creature;
   class Object;


   class Place: public Entity {

      private:

         /*
            Processes the insertion of indexes by alias.

            Input:
               Thing pointer

            Output:
               (none)
         */
         // TODO: try to templatize these?
         void insertThingByName(Thing *thing);
         void insertThingByName(Player *thing);
         void insertThingByName(Creature *thing);
         void insertThingByName(Object *thing);

      protected:

         // sequential list of all entities in a place
         ThingList    things;
         BeingList    beings;
         ItemList     items;
         PlayerList   players;
         CreatureList creatures;
         ObjectList   objects;

         // name -> entity list mappings (accomodates synonyms)
         ThingsByNameMap    thingsByName;
         BeingsByNameMap    beingsByName;
         ItemsByNameMap     itemsByName;
         PlayersByNameMap   playersByName;
         CreaturesByNameMap creaturesByName;
         ObjectsByNameMap   objectsByName;

      public:

         /*
            Constructor for creating a new Place.  Requires reference to the
            containing Game object and a name.
         */
         inline Place(Game *g, string n): Entity(g, n) {}

         /*
            Inserts a Thing that resides inside the Place.  An example would
            be an Object inside a Room.

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void insertThing(Thing *thing);
   };
}}


#endif

