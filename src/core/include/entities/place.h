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
         void insertThingByName(Being *thing);
         void insertThingByName(Player *thing);
         void insertThingByName(Creature *thing);
         void insertThingByName(Object *thing);

         void removeThingByName(Thing *thing);

      protected:

         // sequential list of all entities in a place
         ThingList    things;
         BeingList    beings;
         PlayerList   players;
         CreatureList creatures;
         ObjectList   objects;

         // name -> entity list mappings (accomodates synonyms)
         ThingsByNameMap    thingsByName;
         BeingsByNameMap    beingsByName;
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

         /*
            Indexes a Thing's alias so that it can be referenced by
            name.

            Input:
               Alias (string)
               Thing to be indexed (Thing *)

            Output:
               (none)
         */
         inline void indexThingName(string alias, Thing *thing) {

            if (thingsByName.find(alias) == thingsByName.end()) {
               ThingList newList;
               thingsByName[alias] = newList;
            }

            thingsByName.find(alias)->second.push_back(thing);
         }

         /*
            Indexes a Being's alias so that it can be referenced by
            name.

            Input:
               Alias (string)
               Being to be indexed (Being *)

            Output:
               (none)
         */
         inline void indexBeingName(string alias, Being *being) {

            if (beingsByName.find(alias) == beingsByName.end()) {
               BeingList newList;
               beingsByName[alias] = newList;
            }

            beingsByName.find(alias)->second.push_back(being);
         }

      public:

         /*
            Constructor for creating a new Place.  Requires reference to the
            containing Game object and a name.
         */
         Place(Game *g, string n, Trogout *o, Trogin *i, Trogout *e);

         /*
            Constructor for cloning an existing Place.  Requires a unique name.

            NOTE: Objects that are in a Place will not be copied, in order to
            maintain sanity.
         */
         inline Place(const Place &p, string n): Entity(p, n) {}

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
            Removes a Thing from the Place.

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void removeThing(Thing *thing);

         /*
            Returns iterators over a list of all Things that match the given
            name.  If there are no matches, iterators.begin == iterators.end.

            Input:
               name (string)

            Output:
               begin and end iterators over list of Things (ThingListCItPair)
         */
         inline ThingListCItPair getThingsByName(string name) const {

            ThingListCItPair things;
            ThingsByNameMap::const_iterator i = thingsByName.find(name);

            if (i == thingsByName.end()) {
               things.begin = emptyThingList.begin();
               things.end   = emptyThingList.end();
            }

            else {
               things.begin = i->second.begin();
               things.end   = i->second.end();
            }

            return things;
         }

         /*
            Returns iterators over a list of all Beings that match the given
            name.  If there are no matches, iterators.begin == iterators.end.

            Input:
               name (string)

            Output:
               begin and end iterators over list of Beings (BeingListCItPair)
         */
         inline BeingListCItPair getBeingsByName(string name) const {

            BeingListCItPair beings;
            BeingsByNameMap::const_iterator i = beingsByName.find(name);

            if (i == beingsByName.end()) {
               beings.begin = emptyBeingList.begin();
               beings.end   = emptyBeingList.end();
            }

            else {
               beings.begin = i->second.begin();
               beings.end   = i->second.end();
            }

            return beings;
         }

         /*
            Returns const_iterator to things.begin() and things.end(), so
            that something outside can iterate over all Things in a Place
            without modifying them.

            Input:
               name (string)

            Output:
               things.begin() and things.end() (ThingListCItPair)
         */
         inline ThingListCItPair getThings() const {

            ThingListCItPair iterators;

            iterators.begin = things.begin();
            iterators.end   = things.end();

            return iterators;
         }

         /*
            Returns const_iterator to creatures.begin() and creatures.end(), so
            that something outside can iterate over all Creatures in a Place
            without modifying them.

            Input:
               name (string)

            Output:
               creatures.begin() and creatures.end() (CreatureListCItPair)
         */
         inline CreatureListCItPair getCreatures() const {

            CreatureListCItPair iterators;

            iterators.begin = creatures.begin();
            iterators.end   = creatures.end();

            return iterators;
         }
   };
}}


#endif

