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

         /*
            Overrides Entity::display() and shows a Place's description in the
            proper format.

            Input:
               Being observing the Place
               Whether or not to always show the long description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

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

         /*
            Returns pointer to a list of all Things that match the given name,
            or a NULL pointer if there are no matches.

            Input:
               name (string)

            Output:
               ThingList * (NULL if no matches)
         */
         inline ThingList *getThingsByName(string name) {

            ThingsByNameMap::iterator i = thingsByName.find(name);

            if (i == thingsByName.end()) {
               return 0;
            }

            else {
               return &(i->second);
            }
         }
   };
}}


#endif

