#ifndef ENTITY_H
#define ENTITY_H


#include <any>
#include <list>
#include <set>
#include <memory>
#include <regex>
#include <unordered_set>

#include <trogdor/game.h>

#include <trogdor/entities/type.h>
#include <trogdor/messages.h>

#include <trogdor/lua/luatable.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/event/eventlistener.h>

#include <trogdor/iostream/trogin.h>
#include <trogdor/iostream/trogout.h>
#include <trogdor/iostream/trogerr.h>


namespace trogdor::entity {


   class Place;
   class Room;
   class Thing;
   class Object;
   class Being;
   class Player;
   class Creature;   


   // Defines a valid entity or entity class name
   static const char *validEntityNameRegex = "^[A-Za-z0-9_-]+$";

   /***************************************************************************/

   class Entity: public std::enable_shared_from_this<Entity> {

      private:

         // Custom messages that should be displayed for certain events that act
         // on or with the Entity
         Messages msgs;

         // maintains a list of all Beings that have glanced at but not fully
         // observed the Entity (cannot use unordered_set here because you can't
         // hash a weak_ptr.)
         std::set<
            std::weak_ptr<Being>,
            std::owner_less<std::weak_ptr<Being>>
         > glancedByMap;

         // maintains a list of all Beings that have observed the Entity (cannot
         // use unordered_set here because you can't hash a weak_ptr.)
         std::set<
            std::weak_ptr<Being>,
            std::owner_less<std::weak_ptr<Being>>
         > observedByMap;

         // Entity tags are labels that are either set or not set and are an
         // easy method of categorization
         std::unordered_set<std::string> tags;

         // meta data associated with the entity
         std::unordered_map<std::string, std::string> meta;

      protected:

         std::mutex mutex;

         // every kind of Entity that we are by virtue of inheritance
         std::list<enum EntityType> types;

         // Pointer to the Game that contains the Entity (set to nullptr when
         // the Entity is removed)
         Game *game;

         std::string name;
         std::string className;

         std::string title;
         std::string longDesc;
         std::string shortDesc;

         std::unique_ptr<event::EventListener> triggers;
         std::shared_ptr<LuaState> L;

         // Input and output streams
         std::unique_ptr<Trogout> outStream;
         std::unique_ptr<Trogerr> errStream;
         std::unique_ptr<Trogin> inStream;

         // One or more callbacks that will be executed when various operations
         // occur on the Entity.
         std::unordered_map<
            std::string,
            std::vector<std::shared_ptr<std::function<void(std::any)>>>
         > callbacks;

         // Ordinarily, the lifetime of an Entity is managed by an instance of
         // Game via std::shared_ptrs. However, if an Entity is created in a Lua
         // state and is never assigned to a Game, Lua will manage it instead.
         // This boolean flag lets me know whether or not to hold Lua's garbage
         // collector responsible for a particular instance.
         bool managedByLua = false;

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
         inline void display(const std::shared_ptr<Being> &being, bool displayFull = false) {display(being.get(), displayFull);}

         /*
            Displays the short description of an Entity.  This may be
            overridden by Entity types that have a different display format.

            Input:
               Being doing the observing

            Output:
               (none)
         */
         virtual void displayShort(Being *observer);
         inline void displayShort(const std::shared_ptr<Being> &being) {displayShort(being.get());}

      public:

         // If no output channel is specified when output is sent to the
         // Entity's output stream, this is the channel we use
         static const char *DEFAULT_OUTPUT_CHANNEL;

         /*
            Returns true if the given entity or class name is valid and false
            otherwise.

            Input:
               Entity or class name (std::string)

            Output:
               True if the name is valid and false if not
         */
         static inline bool isNameValid(std::string name) {

            return std::regex_match(name, std::regex(validEntityNameRegex)) ? true : false;
         }

         /*
            Returns the EntityType corresponding to the given string.

            Input:
               Entity type name (std::string)

            Output:
               Entity type (enum EntityType)
         */
         static inline enum EntityType strToType(std::string typeName) {

            // Maps name of Entity type to enum EntityType (initialized in entity.cpp)
            static std::unordered_map<std::string, enum EntityType> strToTypeMap = {
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
               Type name (std::string)
         */
         static inline std::string typeToStr(enum EntityType e) {

            // Maps enum EntityType to its name (initialized in entity.cpp)
            // See that std::hash thing? Yeah, I banged my head against the wall
            // on that for a while. See: 
            // https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
            static std::unordered_map<enum EntityType, std::string, std::hash<int>> typeToStrMap = {
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
               Name (std::string)
               Pointer to output stream object (Trogout *)
               Pointer to input stream object (Trogin *)
               Pointer to error stream object (Trogerr *)
         */
         Entity(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e);

         /*
            Constructor for cloning an Entity into another. IMPORTANT: this does
            NOT automatically assign the entity to a game. A call to
            game->insertEntity() must still be made, and it's possible, if the
            name isn't unique, that doing so will throw an error.

            Input:
               Reference to entity to copy
               Name of copy (std::string)
         */
         Entity(const Entity &e, std::string n);

         /*
            Returns a smart pointer representing a raw Entity pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Entity>
         */
         inline std::shared_ptr<Entity> getShared() {return shared_from_this();}

         /*
            Entity Destructor.

            Input:
               (none)
         */
         virtual ~Entity() = 0;

         /*
            Returns true if the Entity's lifetime is managed by Lua's garbage
            collector and false if it's not.

            Input:
               (none)

            Output:
               Whether or not Entity's lifetime is managed by Lua (bool)
         */
         inline bool isManagedByLua() {return managedByLua;}

         /*
            Sets whether or not the Entity's lifetime should be managed by Lua.

            Input:
               bool

            Output:
               (none)
         */
         inline void setManagedByLua(bool flag) {

            mutex.lock();
            managedByLua = flag;
            mutex.unlock();
         }

         /*
            Adds a callback that should be called when a certain operation
            occurs on the Entity.

            Input:
               Operation the callback should be attached to (std::string)
               Callback (std::shared_ptr<std::function<void(std::any)>>)

            Output:
               (none)
         */
         void addCallback(
            std::string operation,
            std::shared_ptr<std::function<void(std::any)>> callback
         );

         /*
            Removes all callbacks associated with the specified operations and
            returns the number of callbacks removed.

            Input:
               Operation whose callbacks should be removed (std::string)

            Output:
               Number of callbacks removed (size_t)
         */
         size_t removeCallbacks(std::string operation);

         /*
            Removes the specific callback associated with the specified
            operation. This method is why I chose to store shared_ptrs instead
            of the function itself (std::functions can't be compared.)

            Input:
               Operation whose callbacks should be removed (std::string)
               The specific callback to remove (const std::shared_ptr<std::function<void(std::any)>> &)

            Output:
               (none)
         */
         void removeCallback(
            std::string operation,
            const std::shared_ptr<std::function<void(std::any)>> &callback
         );

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

            entityPtr->out() << "I'm a value!" << std::endl;

            Input:
               Output stream channel

            Output:
               Trogout &
         */
         Trogout &out(std::string channel = DEFAULT_OUTPUT_CHANNEL) {

            mutex.lock();
            outStream->setChannel(channel);
            mutex.unlock();

            return *outStream;
         }

         /*
            Returns a reference to the Entity's error stream.  A typical use
            would look something like this:

            entityPtr->err() << "I'm a value!" << std::endl;

            Input:
               Error severity level (default: ERROR)

            Output:
               Trogerr &
         */
         Trogerr &err(Trogerr::ErrorLevel severity = Trogerr::ERROR) {

            mutex.lock();
            errStream->setErrorLevel(severity);
            mutex.unlock();

            return *errStream;
         }

         /*
            Returns a pointer to the Game that contains the Entity.

            Input:
               (none)

            Output:
               Game *
         */
         inline Game *getGame() {return game;}

         /*
            Should only be called when an Entity is inserted into a Game or when
            an Entity is removed from a Game.

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
               Type name (std::string)
         */
         inline std::string getTypeName() const {return typeToStr(types.back());}

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
            Returns true if the given tag is set and false otherwise.

            Input:
               Tag (std::string)

            Output:
               Whether or not the tag is set (bool)
         */
         inline bool isTagSet(std::string tag) const {

            return tags.end() != tags.find(tag) ? true : false;
         }

         /*
            Gets a meta data value.  If the value isn't set, an empty string is
            returned.

            Input:
               meta key (std::string)

            Output:
               meta value (std::string)
         */
         inline std::string getMeta(std::string key) const {

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
               Entity's class (std::string)
         */
         inline std::string getClass() const {return className;}

         /*
            Returns the Entity's name.

            Input:
               (none)

            Output:
               Entity's name (std::string)
         */
         inline std::string getName() const {return name;}

         /*
            Returns the Entity's title.

            Input:
               (none)

            Output:
               Entity's title (std::string)
         */
         inline std::string getTitle() const {return title;}

         /*
            Returns the Entity's long description.

            Input:
               (none)

            Output:
               Entity's long description (std::string)
         */
         inline std::string getLongDescription() const {return longDesc;}

         /*
            Returns the Entity's short description.

            Input:
               (none)

            Output:
               Entity's short description (std::string)
         */
         inline std::string getShortDescription() const {return shortDesc;}

         /*
            Returns a reference to Entity's LuaState object. This should ONLY be
            used by the instantiator and (possibly) other select classes that
            need to interact directly with this. THIS IS A DANGEROUS METHOD. You
            have been warned.

            Input:
               (none)

            Output:
               Game's Lua State (std::shared_ptr<LuaState> &)
         */
         inline std::shared_ptr<LuaState> &getLuaState() {return L;}

         /*
            Returns reference to entity's event listener.

            Input:
               (none)

            Output:
               Reference to event listener (EventListener &)
         */
         inline event::EventListener *getEventListener() {return triggers.get();}

         /*
            Returns whether or not a given Being has observed the Entity.

            Input:
               Being that may have observed the Entity

            Output:
               bool
         */
         inline bool observedBy(const std::shared_ptr<Being> &b) {

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
         inline bool glancedBy(const std::shared_ptr<Being> &b) {

            if (observedByMap.find(b) == observedByMap.end() &&
            glancedByMap.find(b) == glancedByMap.end()) {
               return false;
            }

            return true;
         }

         /*
            Returns the specified message.  If it doesn't exist, an empty string
            is returned.

            Input:
               Message name (std::string)

            Output:
               Message (std::string)
         */
         inline std::string getMessage(std::string message) {return msgs.get(message.c_str());}

         /*
            Passes through to msgs.set()

            Input:
               Message name (std::string)
               Message (std::string)

            Output:
               (none)
         */
         inline void setMessage(std::string name, std::string message) {

            mutex.lock();
            msgs.set(name, message);
            mutex.unlock();
         }

         /*
            Sets a meta data value.

            Input:
               meta key (std::string)
               value (std::string)

            Output:
               (none)
         */
         inline void setMeta(std::string key, std::string value) {

            mutex.lock();
            meta[key] = value;
            mutex.unlock();
         }

         /*
            Sets the Entity's class.

            Input:
               New class (std::string)

            Output:
               (none)
         */
         // TODO: virtual in Thing that will remove/add alias for old and new class name
         inline void setClass(std::string c) {

            mutex.lock();
            className = c;
            mutex.unlock();
         }

         /*
            Sets the Entity's title.

            Input:
               New title (std::string)

            Output:
               (none)
         */
         inline void setTitle(std::string t) {

            mutex.lock();
            title = t;
            mutex.unlock();
         }

         /*
            Sets the Entity's long description.

            Input:
               New long description (std::string)

            Output:
               (none)
         */
         inline void setLongDescription(std::string d) {

            mutex.lock();
            longDesc = d;
            mutex.unlock();
         }

         /*
            Sets the Entity's short description.

            Input:
               New description (std::string)

            Output:
               (none)
         */
         inline void setShortDescription(std::string d) {

            mutex.lock();
            shortDesc = d;
            mutex.unlock();
         }

         /*
            Sets a tag. This is virtual so that other Entity's can wrap around
            it and monitor for changes in tags that are relevant to their state.

            Input:
               Tag (std::string)

            Output:
               (none)
         */
         virtual void setTag(std::string tag);

         /*
            Removes a tag. This is virtual so that other Entity's can wrap
            around it and monitor for changes in tags that are relevant to their
            state.

            Input:
               Tag (std::string)

            Output:
               (none)
         */
         virtual void removeTag(std::string tag);

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
         virtual void observe(const std::shared_ptr<Being> &observer, bool triggerEvents = true,
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
         virtual void glance(const std::shared_ptr<Being> &observer, bool triggerEvents = true);

         /*
            Static method that takes as input iterators over a list of Entities
            that match a given name or alias, asks the Player for clarification
            if there's more than one and returns the chosen Entity.

            Template Arguments:
               ContainerType  -- iterable container type that contains entities
               ResultType     -- one of Entity, Thing, Being, etc.

            Input:
               Iterators over entities to choose from
               Pointer to Entity representing the user (probably a Player)

            Output:
               The chosen entity

            Exceptions:
               If the user types a name that isn't presented as a choice, the
               name the user provided is thrown as an exception.
         */
         template <typename ContainerType, typename ResultType>
         static ResultType clarifyEntity(ContainerType items, Entity *user);
   };

   /***************************************************************************/

   // used to order sets of Entities or Entity pointers alphabetically
   class EntityAlphaComparator {

      public:

         // Compares two Entity objects
         inline bool operator()(const Entity &lhs, const Entity &rhs) const {
            return lhs.getName() < rhs.getName();
         }

         // Compares two raw Entity pointers
         inline bool operator()(const Entity* const &lhs, const Entity* const &rhs) const {
            return (*this)(*lhs, *rhs);
         }

         // Compares two Entity smart pointers
         inline bool operator()(
            const std::shared_ptr<Entity> &lhs,
            const std::shared_ptr<Entity> &rhs) const {
            return (*this)(*lhs, *rhs);
         }
   };

   /***************************************************************************/

   typedef std::list<Place *>    PlaceList;
   typedef std::list<Room *>     RoomList;
   typedef std::list<Thing *>    ThingList;
   typedef std::list<Being *>    BeingList;
   typedef std::list<Player *>   PlayerList;
   typedef std::list<Creature *> CreatureList;
   typedef std::list<Object *>   ObjectList;

   typedef std::unordered_map<std::string, ThingList>    ThingsByNameMap;
   typedef std::unordered_map<std::string, BeingList>    BeingsByNameMap;
   typedef std::unordered_map<std::string, PlayerList>   PlayersByNameMap;
   typedef std::unordered_map<std::string, CreatureList> CreaturesByNameMap;
   typedef std::unordered_map<std::string, ObjectList>   ObjectsByNameMap;

   /***************************************************************************/

   // Special empty lists used for returning results when no result exists
   extern PlaceList     emptyPlaceList;
   extern RoomList      emptyRoomList;
   extern ThingList     emptyThingList;
   extern BeingList     emptyBeingList;
   extern PlayerList    emptyPlayerList;
   extern CreatureList  emptyCreatureList;
   extern ObjectList    emptyObjectList;

   /***************************************************************************/

   template <typename ContainerType, typename ResultType>
   ResultType Entity::clarifyEntity(ContainerType objects, Entity *user) {

      auto i = objects.begin();

      if (i == objects.end()) {
         return 0;
      }

      // pre-increment for looking ahead by one
      else if (++i == objects.end()) {
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
         for (i = objects.begin(); i != objects.end(); ) {

            user->out() << (*i)->getName();
            i++;

            // hackety hack
            if (i == objects.end()) {
               break;
            }

            // temporary lookahead on a bi-directional const_iterator
            else if (++i == objects.end()) {
               user->out() << " or the ";
               i--;
            }

            // pre-decrement to undo temporary lookahead
            else if (--i != objects.begin()) {
               user->out() << ", ";
            }
         }

         user->out() << "?" << std::endl;

         user->out("prompt") << "\n> ";
         user->out("prompt").flush();

         std::string answer;
         user->in() >> answer;

         for (i = objects.begin(); i != objects.end(); i++) {
            if (0 == answer.compare((*i)->getName())) {
               return *i;
            }
         }

         // user typed a name that wasn't in the list
         throw answer;
      }
   }
}


#endif
