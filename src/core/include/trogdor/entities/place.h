#ifndef PLACE_H
#define PLACE_H


#include <list>
#include <cstring>
#include <sstream>
#include <memory>
#include <unordered_map>

#include <trogdor/entities/entity.h>


namespace trogdor::entity {


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
         inline void insertThingByName(std::shared_ptr<Thing> thing) {insertThingByName(thing.get());}
         inline void insertThingByName(std::shared_ptr<Being> being) {insertThingByName(being.get());}
         inline void insertThingByName(std::shared_ptr<Player> player) {insertThingByName(player.get());}
         inline void insertThingByName(std::shared_ptr<Creature> creature) {insertThingByName(creature.get());}
         inline void insertThingByName(std::shared_ptr<Object> object) {insertThingByName(object.get());}

         void removeThingByName(Thing *thing);
         inline void removeThingByName(std::shared_ptr<Thing> thing) {removeThingByName(thing.get());}

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
               Alias (std::string)
               Thing to be indexed (Thing *)

            Output:
               (none)
         */
         inline void indexThingName(std::string alias, Thing *thing) {

            if (thingsByName.find(alias) == thingsByName.end()) {
               ThingList newList;
               thingsByName[alias] = newList;
            }

            thingsByName.find(alias)->second.push_back(thing);
         }

         inline void indexThingName(std::string alias, std::shared_ptr<Thing> thing) {

            indexThingName(alias, thing.get());
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
         inline void indexBeingName(std::string alias, Being *being) {

            if (beingsByName.find(alias) == beingsByName.end()) {
               BeingList newList;
               beingsByName[alias] = newList;
            }

            beingsByName.find(alias)->second.push_back(being);
         }

         inline void indexBeingName(std::string alias, std::shared_ptr<Being> being) {

            indexBeingName(alias, being.get());
         }

      public:

         /*
            Constructor for creating a new Place.  Requires reference to the
            containing Game object and a name.
         */
         Place(Game *g, std::string n, std::unique_ptr<Trogout> o, std::unique_ptr<Trogerr> e);

         /*
            Constructor for cloning an existing Place.  Requires a unique name.

            NOTE: Objects that are in a Place will not be copied, in order to
            maintain sanity.
         */
         inline Place(const Place &p, std::string n): Entity(p, n) {}

         /*
            Inserts a Thing that resides inside the Place.  An example would
            be an Object inside a Room.

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void insertThing(Thing *thing);
         inline void insertThing(std::shared_ptr<Thing> thing) {insertThing(thing.get());}

         /*
            Removes a Thing from the Place.

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void removeThing(Thing *thing);
         inline void removeThing(std::shared_ptr<Thing> thing) {removeThing(thing.get());}

         /*
            Returns iterable list of Things that match the given name.

            Input:
               name (std::string)

            Output:
               const ThingList &
         */
         inline const ThingList &getThingsByName(std::string name) const {

            ThingsByNameMap::const_iterator i = thingsByName.find(name);
            return i == thingsByName.end() ? emptyThingList : i->second;
         }

         /*
            Returns iterable list of Beings that match the given name.

            Input:
               name (std::string)

            Output:
               const BeingList &
         */
         inline const BeingList &getBeingsByName(std::string name) const {

            BeingsByNameMap::const_iterator i = beingsByName.find(name);
            return i == beingsByName.end() ? emptyBeingList : i->second;
         }

         /*
            Returns iterable list of all Things in a Place.

            Input:
               (None)

            Output:
               const ThingList &
         */
         inline const ThingList &getThings() const {return things;}

         /*
            Returns iterable list of all Creatures in a Place.

            Input:
               (None)

            Output:
               const CreatureList &
         */
         inline const CreatureList &getCreatures() const {return creatures;}
   };
}


#endif
