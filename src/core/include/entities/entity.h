#ifndef ENTITY_H
#define ENTITY_H


#include "../messages.h"
#include "../luatable.h"
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
               Whether or not to always display the full description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

         /*
            Displays the short description of an Entity.  This may be
            overridden by Entity types that have a different display format.

            Input:
               Being doing the observing

            Output:
               (none)
         */
         virtual void displayShort(Being *observer);

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
            Constructor for cloning an Entity into another (with a unique name.)

            Input:
               Reference to entity to copy
               Name of copy (string)
         */
         Entity(const Entity &e, string n);

         /*
            Returns a LuaTable object representing the Entity.  Note that each
            child class should get the value of the parent class's version of
            this method and then fill that object in further with its own
            values.

            Input:
               (none)

            Output:
               LuaTable object
         */
         virtual LuaTable *getLuaTable() const;

         /*
            Returns the Entity's type.

            Input:
               (none)

            Output:
               (none)
         */
         inline enum EntityType getType() const {return type;}

         /*
            Returns a string representation of the Entity's type.

            Input:
               (none)

            Output:
               Type name (string)
         */
         inline string getTypeName() const {

            switch (type) {

               case ENTITY_ROOM:
                  return "room";

               case ENTITY_OBJECT:
                  return "object";

               case ENTITY_CREATURE:
                  return "creature";

               case ENTITY_PLAYER:
                  return "player";

               default:
                  return "undefined";
            }
         }

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
            Returns Entity's EventListener.

            Input:
               (none)

            Output:
               Pointer to EventListener
         */
         inline EventListener *getEventListener() const {return triggers;}

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
            Returns the specified message.  If it doesn't exist, an empty string
            is returned.

            Input:
               Message name (string)

            Output:
               Message (string)
         */
         inline string getMessage(string message) {return msgs.get(message.c_str());}

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
               Whether or not to always display the long description

            Output:
               (none)

            Events Triggered:
               beforeObserve
               afterObserve
         */
         virtual void observe(Being *observer, bool triggerEvents = true,
         bool displayFull = false);

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

         /*
            Static method that takes as input a list of Entities that match a
            given word, asks the Player for clarification and returns the chosen
            Entity.

            Template Arguments:
               ListType   -- one of EntityList, ThingList, BeingList, etc.
               ResultType -- one of Entity, Thing, Being, etc.

            Input:
               List of objects to choose from
               Input stream
               Output stream

            Output:
               The chosen object

            Exceptions:
               If the user types a name that isn't presented as a choice, the
               name the user provided is thrown as an exception.
         */
         template <typename ListType, typename ResultType>
         static ResultType clarifyEntity(ListType items, istream *trogin,
            ostream *trogout);
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

   typedef PlaceList::const_iterator    PlaceListCIt;
   typedef RoomList::const_iterator     RoomListCIt;
   typedef ThingList::const_iterator    ThingListCIt;
   typedef BeingList::const_iterator    BeingListCIt;
   typedef ItemList::const_iterator     ItemListCIt;
   typedef PlayerList::const_iterator   PlayerListCIt;
   typedef CreatureList::const_iterator CreatureListCIt;
   typedef ObjectList::const_iterator   ObjectListCIt;

   typedef struct {
      PlaceListCIt begin;
      PlaceListCIt end;
   } PlaceListCItPair;

   typedef struct {
      RoomListCIt begin;
      RoomListCIt end;
   } RoomListCItPair;

   typedef struct {
      ThingListCIt begin;
      ThingListCIt end;
   } ThingListCItPair;

   typedef struct {
      BeingListCIt begin;
      BeingListCIt end;
   } BeingListCItPair;

   typedef struct {
      ItemListCIt begin;
      ItemListCIt end;
   } ItemListCItPair;

   typedef struct {
      PlayerListCIt begin;
      PlayerListCIt end;
   } PlayerListCItPair;

   typedef struct {
      CreatureListCIt begin;
      CreatureListCIt end;
   } CreatureListCItPair;

   typedef struct {
      ObjectListCIt begin;
      ObjectListCIt end;
   } ObjectListCItPair;

   typedef PlaceList::iterator    PlaceListIt;
   typedef RoomList::iterator     RoomListIt;
   typedef ThingList::iterator    ThingListIt;
   typedef BeingList::iterator    BeingListIt;
   typedef ItemList::iterator     ItemListIt;
   typedef PlayerList::iterator   PlayerListIt;
   typedef CreatureList::iterator CreatureListIt;
   typedef ObjectList::iterator   ObjectListIt;

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

   /***************************************************************************/

   // static method
   template <typename ListType, typename ResultType>
   ResultType Entity::clarifyEntity(ListType items, istream *trogin,
   ostream *trogout) {

      int count = items.size();

      if (0 == count) {
         return 0;
      }

      else if (1 == count) {
         return items.front();
      }

      else {

         *trogout << "Do you mean the ";

         int c = 0;
         for (typename ListType::iterator i = items.begin(); i != items.end();
         i++, c++) {

            *trogout << (*i)->getName();

            if (c < items.size() - 2) {
               *trogout << ", ";
            }

            else if (c < items.size() - 1) {
               *trogout << " or the ";
            }
         }

         *trogout << "? \n\n> ";

         string answer;
         getline(*trogin, answer);

         for (typename ListType::iterator i = items.begin(); i != items.end(); i++) {
            if (0 == answer.compare((*i)->getName())) {
               return *i;
            }
         }

         // user typed a name that wasn't in the list
         throw answer;
      }
   }
}}


#endif

