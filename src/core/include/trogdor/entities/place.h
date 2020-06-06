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
         Place(const Place &p, std::string n);

         /*
            Returns a smart pointer representing a raw Place pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Place>
         */
         inline std::shared_ptr<Place> getShared() {

            return std::dynamic_pointer_cast<Place>(Entity::getShared());
         }

         /*
            Inserts a Player that resides inside the Place.

            Input:
               Player pointer

            Output:
               (none)
         */
         void insertPlayer(Player *thing);

         /*
            Inserts a Creature that resides inside the Place.

            Input:
               Creature pointer

            Output:
               (none)
         */
         void insertCreature(Creature *thing);

         /*
            Inserts an Object that's contained inside the Place. This Object
            should NOT be in a Being's inventory.

            Input:
               Object pointer

            Output:
               (none)
         */
         void insertObject(Object *thing);

         /*
            Inserts any Thing that resides/is contained inside the Place (calls
            one of insertPlayer, insertCreature, or insertObject.)

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void insertThing(Thing *thing);
         inline void insertThing(std::shared_ptr<Thing> thing) {insertThing(thing.get());}

         /*
            Removes a Player from the Place.

            Input:
               Pointer to Player

            Output:
               (none)
         */
         void removePlayer(Player *player);

         /*
            Removes a Creature from the Place.

            Input:
               Pointer to Creature

            Output:
               (none)
         */
         void removeCreature(Creature *creature);

         /*
            Removes an Object from the Place.

            Input:
               Pointer to Object

            Output:
               (none)
         */
         void removeObject(Object *object);

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
