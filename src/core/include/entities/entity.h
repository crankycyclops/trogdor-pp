#ifndef ENTITY_H
#define ENTITY_H


#include <list>
#include <set>
#include <memory>
#include <algorithm>
#include <regex>

#include "../messages.h"
#include "../lua/luatable.h"
#include "../lua/luastate.h"
#include "../iostream/trogout.h"
#include "../iostream/trogin.h"


namespace trogdor {

   class Game;
   class Parser;

   namespace event {

      class EventListener;
   }
}

using namespace trogdor::event;

namespace trogdor { namespace entity {


   class Place;
   class Room;
   class Thing;
   class Object;
   class Being;
   class Player;
   class Creature;   


   // used for run-time check of an Entity's type
   enum EntityType {
      ENTITY_UNDEFINED = 0,
      ENTITY_ENTITY = 1,
      ENTITY_PLACE = 2,
      ENTITY_ROOM = 3,
      ENTITY_THING = 4,
      ENTITY_OBJECT = 5,
      ENTITY_BEING = 6,
      ENTITY_PLAYER = 7,
      ENTITY_CREATURE = 8,
   };

   // Defines a valid entity or entity class name
   static const char *validEntityNameRegex = "^[A-Za-z0-9_-]+$";

   /***************************************************************************/

   class Entity {

      protected:

         // every kind of Entity that we are by virtue of inheritance
         list<enum EntityType> types;

         Game *game;

         std::unique_ptr<Trogout> outStream;
         std::unique_ptr<Trogout> errStream;
         std::unique_ptr<Trogin> inStream;

         string className;
         string name;
         string title;
         string longDesc;
         string shortDesc;

         Messages msgs;

         std::shared_ptr<LuaState> L;
         std::unique_ptr<EventListener> triggers;

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
         inline void display(std::shared_ptr<Being> being, bool displayFull = false) {display(being.get(), displayFull);}

         /*
            Displays the short description of an Entity.  This may be
            overridden by Entity types that have a different display format.

            Input:
               Being doing the observing

            Output:
               (none)
         */
         virtual void displayShort(Being *observer);
         inline void displayShort(std::shared_ptr<Being> being) {displayShort(being.get());}

      public:

         /*
            Returns true if the given entity or class name is valid and false
            otherwise.

            Input:
               Entity or class name (string)

            Output:
               True if the name is valid and false if not
         */
         static inline bool isNameValid(string name) {

            return regex_match(name, regex(validEntityNameRegex)) ? true : false;
         }

         /*
            Returns the EntityType corresponding to the given string.

            Input:
               Entity type name (string)

            Output:
               Entity type (enum EntityType)
         */
         static inline enum EntityType strToType(string typeName) {

            // Maps name of Entity type to enum EntityType (initialized in entity.cpp)
            static unordered_map<string, enum EntityType> strToTypeMap = {
               {"entity", ENTITY_ENTITY},
               {"place", ENTITY_PLACE},
               {"room", ENTITY_ROOM},
               {"thing", ENTITY_THING},
               {"being", ENTITY_BEING},
               {"object", ENTITY_OBJECT},
               {"creature", ENTITY_CREATURE},
               {"player", ENTITY_PLAYER}
            };

            return strToTypeMap.find(typeName) != strToTypeMap.end() ?
               strToTypeMap[typeName] : ENTITY_UNDEFINED;
         }

         /*
            Returns a string representation of the given Entity type.

            Input:
               enum EntityType

            Output:
               Type name (string)
         */
         static inline string typeToStr(enum EntityType e) {

            // Maps enum EntityType to its name (initialized in entity.cpp)
            // See that std::hash thing? Yeah, I banged my head against the wall
            // on that for a while. See: 
            // https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
            static unordered_map<enum EntityType, string, std::hash<int>> typeToStrMap = {
               {ENTITY_ENTITY, "entity"},
               {ENTITY_PLACE, "place"},
               {ENTITY_ROOM, "room"},
               {ENTITY_THING, "thing"},
               {ENTITY_BEING, "being"},
               {ENTITY_OBJECT, "object"},
               {ENTITY_CREATURE, "creature"},
               {ENTITY_PLAYER, "player"}
            };

            return typeToStrMap.find(e) != typeToStrMap.end() ?
               typeToStrMap[e] : "undefined";
         }

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
         Entity(Game *g, string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogout> e);

         /*
            Constructor for cloning an Entity into another (with a unique name.)

            Input:
               Reference to entity to copy
               Name of copy (string)
         */
         Entity(const Entity &e, string n);

         /*
            Entity Destructor. For some reason that mystifies me, this is
            required to compile successfully, even though it's blank (see
            entities/entity.cpp.)

            Input:
               (none)
         */
         ~Entity();

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
            Returns a pointer to the Game that contains the Entity.

            Input:
               (none)

            Output:
               Game *
         */
         inline Game *getGame() {return game;}

         /*
            Should only be called when an Entity created by Lua is later
            inserted into a running game.

            Input:
               Game *

            Output:
               (none)
         */
         void setGame(Game *g);

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

            auto matchedType = find_if(types.begin(), types.end(),
            [&](enum EntityType curType) {
               return type == curType;
            });

            return matchedType == types.end() ? false : true;
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
            Returns the Entity's class.

            Input:
               (none)

            Output:
               Entity's class (string)
         */
         inline string getClass() const {return className;}

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
            Returns a reference to Entity's LuaState object. This should ONLY be
            used by the instantiator and (possibly) other select classes that
            need to interact directly with this. THIS IS A DANGEROUS METHOD. You
            have been warned.

            Input:
               (none)

            Output:
               Game's Lua State (const &LuaState)
         */
         std::shared_ptr<LuaState> &getLuaState() {return L;}

         /*
            Returns raw pointer to Entity's EventListener.
            TODO: make this consistent with Game::getEventListener.

            Input:
               (none)

            Output:
               Pointer to EventListener
         */
         inline EventListener *getEventListener() const {return triggers.get();}

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

         inline bool observedBy(std::shared_ptr<Being> being) {return observedBy(being.get());}

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

         inline bool glancedBy(std::shared_ptr<Being> being) {return glancedBy(being.get());}

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
            Passes through to msgs.set()

            Input:
               Message name (string)
               Message (string)

            Output:
               (none)
         */
         inline void setMessage(string name, string message) {msgs.set(name, message);}

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
            Sets the Entity's class.

            Input:
               New class (string)

            Output:
               (none)
         */
         // TODO: virtual in Thing that will remove/add alias for old and new class name
         inline void setClass(string c) {className = c;}

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

         inline void observe(std::shared_ptr<Being> observer,
         bool triggerEvents = true, bool displayFull = false) {

            observe(observer.get(), triggerEvents, displayFull);
         }

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

         inline void glance(std::shared_ptr<Being> observer,
         bool triggerEvents = true) {

            glance(observer.get(), triggerEvents);
         }

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
   typedef list<Player *>   PlayerList;
   typedef list<Creature *> CreatureList;
   typedef list<Object *>   ObjectList;

   typedef PlaceList::const_iterator    PlaceListCIt;
   typedef RoomList::const_iterator     RoomListCIt;
   typedef ThingList::const_iterator    ThingListCIt;
   typedef BeingList::const_iterator    BeingListCIt;
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
   typedef PlayerList::iterator   PlayerListIt;
   typedef CreatureList::iterator CreatureListIt;
   typedef ObjectList::iterator   ObjectListIt;

   typedef set<Place *, EntityAlphaComparator>    PlaceSet;
   typedef set<Room *, EntityAlphaComparator>     RoomSet;
   typedef set<Thing *, EntityAlphaComparator>    ThingSet;
   typedef set<Being *, EntityAlphaComparator>    BeingSet;
   typedef set<Player *, EntityAlphaComparator>   PlayerSet;
   typedef set<Creature *, EntityAlphaComparator> CreatureSet;
   typedef set<Object *, EntityAlphaComparator>   ObjectSet;

   typedef PlaceSet::const_iterator    PlaceSetCIt;
   typedef RoomSet::const_iterator     RoomSetCIt;
   typedef ThingSet::const_iterator    ThingSetCIt;
   typedef BeingSet::const_iterator    BeingSetCIt;
   typedef PlayerSet::const_iterator   PlayerSetCIt;
   typedef CreatureSet::const_iterator CreatureSetCIt;
   typedef ObjectSet::const_iterator   ObjectSetCIt;

   typedef struct {
      ObjectSetCIt begin;
      ObjectSetCIt end;
   } ObjectSetCItPair;

   typedef unordered_map<string, ThingList>    ThingsByNameMap;
   typedef unordered_map<string, BeingList>    BeingsByNameMap;
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
   extern PlayerList    emptyPlayerList;
   extern CreatureList  emptyCreatureList;
   extern ObjectList    emptyObjectList;

   /***************************************************************************/

   template <typename CItStruct, typename CItType, typename ResultType>
   ResultType Entity::clarifyEntity(CItStruct objects, Entity *user) {

      CItType i = objects.begin;

      if (i == objects.end) {
         return 0;
      }

      // pre-increment for looking ahead by one
      else if (++i == objects.end) {
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
         for (i = objects.begin; i != objects.end; ) {

            user->out() << (*i)->getName();
            i++;

            // hackety hack
            if (i == objects.end) {
               break;
            }

            // temporary lookahead on a bi-directional const_iterator
            else if (++i == objects.end) {
               user->out() << " or the ";
               i--;
            }

            // pre-decrement to undo temporary lookahead
            else if (--i != objects.begin) {
               user->out() << ", ";
            }
         }

         user->out() << "? \n\n> ";
         user->out().flush();

         string answer;
         user->in() >> answer;

         for (i = objects.begin; i != objects.end; i++) {
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

