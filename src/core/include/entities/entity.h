#ifndef ENTITY_H
#define ENTITY_H


#include <list>
#include <set>

#include "../messages.h"
#include "../lua/luatable.h"
#include "../lua/luastate.h"
#include "../iostream/trogout.h"
#include "../iostream/trogin.h"


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
      ENTITY_PLACE = 2,
      ENTITY_ROOM = 3,
      ENTITY_THING = 4,
      ENTITY_ITEM = 5,
      ENTITY_OBJECT = 6,
      ENTITY_BEING = 7,
      ENTITY_PLAYER = 8,
      ENTITY_CREATURE = 9
   };

   /***************************************************************************/

   class Entity {

      protected:

         // every kind of Entity that we are by virtue of inheritance
         list<enum EntityType> types;

         Game *game;

         Trogout *outStream;
         Trogout *errStream;
         Trogin  *inStream;

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

         // meta data associated with the entity
         unordered_map<string, string> meta;

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
               Pointer to output stream object (Trogout *)
               Pointer to input stream object (Trogin *)
               Pointer to error stream object (Trogout *)
         */
         Entity(Game *g, string n, Trogout *o, Trogin *i, Trogout *e);

         /*
            Constructor for cloning an Entity into another (with a unique name.)

            Input:
               Reference to entity to copy
               Name of copy (string)
         */
         Entity(const Entity &e, string n);

         /*
            Returns a reference to the Entity's input stream.  A typical use
            would look something like this:

            entityPtr->in() >> stringVal;

            Input:
               (none)

            Output:
               Trogin &
         */
         Trogin &in() {return *inStream;}

         /*
            Returns a reference to the Entity's output stream.  A typical use
            would look something like this:

            entityPtr->out() << "I'm a value!" << endl;

            Input:
               Output stream channel (default: notifications)

            Output:
               Trogout &
         */
         Trogout &out(string channel = "notifications") {

            outStream->setChannel(channel);
            return *outStream;
         }

         /*
            Returns a reference to the Entity's error stream.  A typical use
            would look something like this:

            entityPtr->err() << "I'm a value!" << endl;

            Input:
               (none)

            Output:
               Trogout &
         */
         Trogout &err() {return *errStream;}

         /*
            Entity Destructor.

            Input:
               (none)
         */
         ~Entity();

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
            Returns a string representation of the given Entity type.

            Input:
               enum EntityType

            Output:
               Type name (string)
         */
         inline string typeToStr(enum EntityType e) const {

            switch (e) {

               case ENTITY_PLACE:
                  return "place";

               case ENTITY_ROOM:
                  return "room";

               case ENTITY_THING:
                  return "thing";

               case ENTITY_ITEM:
                  return "item";

               case ENTITY_BEING:
                  return "being";

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
            Returns the Entity's most specific type.

            Input:
               (none)

            Output:
               (none)
         */
         inline enum EntityType getType() const {return types.back();}

         /*
            Returns a string representation of the Entity's most specific type.

            Input:
               (none)

            Output:
               Type name (string)
         */
         inline string getTypeName() const {return typeToStr(types.back());}

         /*
            Returns true if the Entity is of the given type. Examines the whole
            inheritance hierarchy, and can, for example, tell you if a Creature
            is a Being, Thing, etc.

            Input:
               Entity type (enum EntityType)

            Output:
               true if the Entity is of the given type and false if not
         */
         inline bool isType(enum EntityType type) const {

            for (list<enum EntityType>::const_iterator i = types.begin();
            i != types.end(); i++) {
               if (type == *i) {
                  return true;
               }
            }

            return false;
         }

         /*
            Gets a meta data value.  If the value isn't set, an empty string is
            returned.

            Input:
               meta key (string)

            Output:
               meta value (string)
         */
         inline string getMeta(string key) const {

            if (meta.find(key) == meta.end()) {
               return "";
            }

            return meta.find(key)->second;
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
            Sets a meta data value.

            Input:
               meta key (string)
               value (string)

            Output:
               (none)
         */
         inline void setMeta(string key, string value) {meta[key] = value;}

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
            Static method that takes as input iterators over a list of Entities
            that match a given name or alias, asks the Player for clarification
            if there's more than one and returns the chosen Entity.

            Template Arguments:
               CItStruct  -- struct containing begin and end iterators (one of
                             ThingListCItPair, etc.)
               CItType    -- constant iterator type (one of ThingListCIt, etc.)
               ResultType -- one of Entity, Thing, Being, etc.

            Input:
               Iterators over entities to choose from
               Pointer to Entity representing the user (probably a Player)

            Output:
               The chosen entity

            Exceptions:
               If the user types a name that isn't presented as a choice, the
               name the user provided is thrown as an exception.
         */
         template <typename CItStruct, typename CItType, typename ResultType>
         static ResultType clarifyEntity(CItStruct items, Entity *user);
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

   typedef PlaceSet::const_iterator    PlaceSetCIt;
   typedef RoomSet::const_iterator     RoomSetCIt;
   typedef ThingSet::const_iterator    ThingSetCIt;
   typedef BeingSet::const_iterator    BeingSetCIt;
   typedef ItemSet::const_iterator     ItemSetCIt;
   typedef PlayerSet::const_iterator   PlayerSetCIt;
   typedef CreatureSet::const_iterator CreatureSetCIt;
   typedef ObjectSet::const_iterator   ObjectSetCIt;

   typedef struct {
      ObjectSetCIt begin;
      ObjectSetCIt end;
   } ObjectSetCItPair;

   typedef unordered_map<string, ThingList>    ThingsByNameMap;
   typedef unordered_map<string, BeingList>    BeingsByNameMap;
   typedef unordered_map<string, ItemList>     ItemsByNameMap;
   typedef unordered_map<string, PlayerList>   PlayersByNameMap;
   typedef unordered_map<string, CreatureList> CreaturesByNameMap;
   typedef unordered_map<string, ObjectList>   ObjectsByNameMap;

   /***************************************************************************/

   // Special empty lists used for returning .end() iterator pairs in the case
   // of a non-existent list
   extern PlaceList     emptyPlaceList;
   extern RoomList      emptyRoomList;
   extern ThingList     emptyThingList;
   extern BeingList     emptyBeingList;
   extern ItemList      emptyItemList;
   extern PlayerList    emptyPlayerList;
   extern CreatureList  emptyCreatureList;
   extern ObjectList    emptyObjectList;

   /***************************************************************************/

   template <typename CItStruct, typename CItType, typename ResultType>
   ResultType Entity::clarifyEntity(CItStruct items, Entity *user) {

      CItType i = items.begin;

      if (i == items.end) {
         return 0;
      }

      // pre-increment for looking ahead by one
      else if (++i == items.end) {
         return *(--i);
      }

      else {

         // undo lookahead from above
         i--;

         user->out() << "Do you mean the ";

         // This loop is a little nasty.  The logic was A LOT nicer when I was
         // using a list and could get its size, but I had to hack and clobber
         // the existing loop to make it work with bi-directional iterators
         // where i don't know the number of elements.  I apologize in advance
         // to whoever is forced to read it...
         for (i = items.begin; i != items.end; ) {

            user->out() << (*i)->getName();
            i++;

            // hackety hack
            if (i == items.end) {
               break;
            }

            // temporary lookahead on a bi-directional const_iterator
            else if (++i == items.end) {
               user->out() << " or the ";
               i--;
            }

            // pre-decrement to undo temporary lookahead
            else if (--i != items.begin) {
               user->out() << ", ";
            }
         }

         user->out() << "? \n\n> ";
         user->out().flush();

         string answer;
         user->in() >> answer;

         for (i = items.begin; i != items.end; i++) {
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

