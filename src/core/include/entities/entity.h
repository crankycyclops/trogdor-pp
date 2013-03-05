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


   class Place;
   class Room;
   class Thing;
   class Item;
   class Object;
   class Being;
   class Player;
   class Creature;   


   // used for run-time check of an Entity's type
   enum EntityType {
      ENTITY_UNDEFINED = 1,
      ENTITY_ROOM = 2,
      ENTITY_PLAYER = 3,
      ENTITY_CREATURE = 4,
      ENTITY_OBJECT = 5
   };

   /***************************************************************************/

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

         // maintains a list of all Beings that have glanced at but not fully
         // observed the Entity
         unordered_map<Being *, bool> glancedByMap;

         // maintains a list of all Beings that have observed the Entity
         unordered_map<Being *, bool> observedByMap;

         /*
            Displays an Entity.  This may be overridden by Entity types that
            have a different display format.

            Input:
               Being doing the observing

            Output:
               (none)
         */
         virtual void display(Being *observer);

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
            Returns whether or not a given Being has observed the Entity.

            Input:
               Being that may have observed the Entity

            Output:
               bool
         */
         inline bool observedBy(Being *b) {

            if (observedByMap.find(b) == observedByMap.end()) {
               return false;
            }

            return true;
         }

         /*
            Returns whether or not a given Being has glanced at the Entity.  If
            a Being has fully observed an Entity, it stands to reason that the
            Being has also glanced at it, so both will be checked.

            Input:
               Being that may have glanced at the Entity

            Output:
               bool
         */
         inline bool glancedBy(Being *b) {

            if (observedByMap.find(b) == observedByMap.end() &&
            glancedByMap.find(b) == glancedByMap.end()) {
               return false;
            }

            return true;
         }

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
         inline void setShortDescription(string d) {shortDesc = d;}

         /*
            Gives a Being the ability to observe an Entity.  If the Being is a
            Player, a description of what is seen (Entity's long description)
            will be printed to the output stream.

            Input:
               Being doing the observing
               Whether or not to trigger a before and after event

            Output:
               (none)

            Events Triggered:
               beforeObserve
               afterObserve
         */
         virtual void observe(Being *observer, bool triggerEvents = true);

         /*
            Gives a Being the ability to partially observe an Entity without
            looking straight at it (what you might see during a cursory glance
            of a room, for example.)  If the Being is a Player, the Entity's
            short description will be printed to the output stream.

            Input:
               Being doing the observing
               Whether or not to trigger a before and after event

            Output:
               (none)

            Events Triggered:
               beforeGlance
               afterGlance
         */
         virtual void glance(Being *observer, bool triggerEvents = true);
   };

   /***************************************************************************/

   // used by std::set to order Entities (referenced by pointers) alphabetically
   class EntityAlphaComparator {

      public:

         inline bool operator()(const Entity* const &lhs, const Entity* const &rhs) {
            return lhs->getName() < rhs->getName();
         }
   };

   /***************************************************************************/

   typedef list<Place *>    PlaceList;
   typedef list<Room *>     RoomList;
   typedef list<Thing *>    ThingList;
   typedef list<Being *>    BeingList;
   typedef list<Item *>     ItemList;
   typedef list<Player *>   PlayerList;
   typedef list<Creature *> CreatureList;
   typedef list<Object *>   ObjectList;

   typedef set<Place *, EntityAlphaComparator>    PlaceSet;
   typedef set<Room *, EntityAlphaComparator>     RoomSet;
   typedef set<Thing *, EntityAlphaComparator>    ThingSet;
   typedef set<Being *, EntityAlphaComparator>    BeingSet;
   typedef set<Item *, EntityAlphaComparator>     ItemSet;
   typedef set<Player *, EntityAlphaComparator>   PlayerSet;
   typedef set<Creature *, EntityAlphaComparator> CreatureSet;
   typedef set<Object *, EntityAlphaComparator>   ObjectSet;

   typedef unordered_map<string, ThingList>    ThingsByNameMap;
   typedef unordered_map<string, BeingList>    BeingsByNameMap;
   typedef unordered_map<string, ItemList>     ItemsByNameMap;
   typedef unordered_map<string, PlayerList>   PlayersByNameMap;
   typedef unordered_map<string, CreatureList> CreaturesByNameMap;
   typedef unordered_map<string, ObjectList>   ObjectsByNameMap;
}}


#endif

