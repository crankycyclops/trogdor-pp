#ifndef PLACE_H
#define PLACE_H


#include <vector>
#include <boost/unordered_map.hpp>

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
         vector<Thing *>    things;
         vector<Being *>    beings;
         vector<Item *>     items;
         vector<Player *>   players;
         vector<Creature *> creatures;
         vector<Object *>   objects;

         // name -> entity list mappings (accomodates synonyms)
         unordered_map<string, vector<Thing *> >    thingsByName;
         unordered_map<string, vector<Being *> >    beingsByName;
         unordered_map<string, vector<Item *> >     itemsByName;
         unordered_map<string, vector<Player *> >   playersByName;
         unordered_map<string, vector<Creature *> > creaturesByName;
         unordered_map<string, vector<Object *> >   objectsByName;

      public:
   };
}}


#endif

