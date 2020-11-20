#ifndef PLACE_H
#define PLACE_H


#include <list>
#include <cstring>
#include <sstream>
#include <memory>
#include <unordered_map>

#include <trogdor/entities/tangible.h>


namespace trogdor::entity {


   // Forward declarations to prevent circular dependency issues
   class Thing;
   class Being;
   class Player;
   class Creature;
   class Object;


   class Place: public Tangible {

      protected:

         // sequential list of all entities in a place
         std::list<std::shared_ptr<Thing>>    things;
         std::list<std::shared_ptr<Being>>    beings;
         std::list<std::shared_ptr<Player>>   players;
         std::list<std::shared_ptr<Creature>> creatures;
         std::list<std::shared_ptr<Object>>   objects;

         // alias -> entity list indices
         std::unordered_map<std::string, std::list<Thing *>>    thingsByName;
         std::unordered_map<std::string, std::list<Being *>>    beingsByName;
         std::unordered_map<std::string, std::list<Player *>>   playersByName;
         std::unordered_map<std::string, std::list<Creature *>> creaturesByName;
         std::unordered_map<std::string, std::list<Object *>>   objectsByName;

         /*
            Display all Resources currently allocated to the Place.

            Input:
               Being observing the Place (Being *)

            Output:
               (none)
         */
         void displayResources(Being *observer);

         /*
            Display all Things inside the Place.

            Input:
               Being observing the Place (Being *)

            Output:
               (none)
         */
         void displayThings(Being *observer);

         /*
            Show just the Place's description (called by Place::display.)

            Input:
               Being observing the Place (Being *)
               Whether or not to show the full description (bool)

            Output:
               (none)
         */
         void displayPlace(Being *observer, bool displayFull);

         /*
            Overrides Tangible::display() and shows a Place's description in the
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
            This constructor deserializes a Place.

            Input:
               Game the entity belongs to (Game *)
               Raw deserialized data (const Serializable &)
         */
         Place(Game *g, const serial::Serializable &data);

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
            Serializes the Place.

            Input:
               (none)

            Output:
               An object containing easily serializable data (std::shared_ptr<Serializable>)
         */
         virtual std::shared_ptr<serial::Serializable> serialize();

         /*
            Inserts a Player that resides inside the Place.

            Input:
               Player pointer

            Output:
               (none)
         */
         void insertPlayer(const std::shared_ptr<Player> &player);

         /*
            Inserts a Creature that resides inside the Place.

            Input:
               Creature pointer

            Output:
               (none)
         */
         void insertCreature(const std::shared_ptr<Creature> &creature);

         /*
            Inserts an Object that's contained inside the Place. This Object
            should NOT be in a Being's inventory.

            Input:
               Object pointer

            Output:
               (none)
         */
         void insertObject(const std::shared_ptr<Object> &thing);

         /*
            Inserts any Thing that resides/is contained inside the Place (calls
            one of insertPlayer, insertCreature, or insertObject.)

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void insertThing(const std::shared_ptr<Thing> &thing);

         /*
            Removes a Player from the Place.

            Input:
               Pointer to Player

            Output:
               (none)
         */
         void removePlayer(const std::shared_ptr<Player> &player);

         /*
            Removes a Creature from the Place.

            Input:
               Pointer to Creature

            Output:
               (none)
         */
         void removeCreature(const std::shared_ptr<Creature> &creature);

         /*
            Removes an Object from the Place.

            Input:
               Pointer to Object

            Output:
               (none)
         */
         void removeObject(const std::shared_ptr<Object> &object);

         /*
            Removes a Thing from the Place.

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void removeThing(const std::shared_ptr<Thing> &thing);

         /*
            Returns iterable list of Things that match the given name.

            Input:
               name (std::string)

            Output:
               const std::list<std::shared_ptr<Thing>> &
         */
         inline const auto &getThingsByName(std::string name) const {

            auto i = thingsByName.find(name);
            return i == thingsByName.end() ? emptyThingList : i->second;
         }

         /*
            Returns iterable list of Beings that match the given name.

            Input:
               name (std::string)

            Output:
               const std::list<std::shared_ptr<Being>> &
         */
         inline const auto &getBeingsByName(std::string name) const {

            auto i = beingsByName.find(name);
            return i == beingsByName.end() ? emptyBeingList : i->second;
         }

         /*
            Returns iterable list of all Things in a Place.

            Input:
               (None)

            Output:
               const std::list<std::shared_ptr<Thing>> &
         */
         inline const auto &getThings() const {return things;}

         /*
            Returns iterable list of all Beings in a Place.

            Input:
               (None)

            Output:
               const std::list<std::shared_ptr<Being>> &
         */
         inline const auto &getBeings() const {return beings;}

         /*
            Returns iterable list of all Players in a Place.

            Input:
               (None)

            Output:
               const std::list<std::shared_ptr<Player>> &
         */
         inline const auto &getPlayers() const {return players;}

         /*
            Returns iterable list of all Creatures in a Place.

            Input:
               (None)

            Output:
               const std::list<std::shared_ptr<Creature>> &
         */
         inline const auto &getCreatures() const {return creatures;}
   
            /*
            Returns iterable list of all Objects in a Place.

            Input:
               (None)

            Output:
               const std::list<std::shared_ptr<Object>> &
         */
         inline const auto &getObjects() const {return objects;}
   };
}


#endif
