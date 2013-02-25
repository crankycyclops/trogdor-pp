#ifndef PLACE_H
#define PLACE_H


#include <typeinfo>
#include <list>
#include <cstring>
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

      protected:

         // sequential list of all entities in a place
         list<Thing *>    things;
         list<Being *>    beings;
         list<Item *>     items;
         list<Player *>   players;
         list<Creature *> creatures;
         list<Object *>   objects;

         // name -> entity list mappings (accomodates synonyms)
         unordered_map<string, list<Thing *> >    thingsByName;
         unordered_map<string, list<Being *> >    beingsByName;
         unordered_map<string, list<Item *> >     itemsByName;
         unordered_map<string, list<Player *> >   playersByName;
         unordered_map<string, list<Creature *> > creaturesByName;
         unordered_map<string, list<Object *> >   objectsByName;

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

