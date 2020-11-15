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

#include <trogdor/iostream/trogout.h>
#include <trogdor/iostream/trogerr.h>

namespace trogdor::entity {


   class Resource;
   class Tangible;
   class Place;
   class Room;
   class Thing;
   class Object;
   class Being;
   class Player;
   class Creature;   


   // Defines a valid entity or entity class name
   static constexpr const char *validEntityNameRegex = "^[ A-Za-z0-9_-]+$";

   // An entity name cannot be any of these
   static const char *reservedNames[] = {
      "myself",
      nullptr
   };

   /***************************************************************************/

   class Entity: public std::enable_shared_from_this<Entity> {

      public:

         // Property validators should return this if the value being set is
         // valid
         static constexpr int PROPERTY_VALID = 0;

         // Property validators should return this if attempting to set a
         // property value with an invalid type
         static constexpr int PROPERTY_INVALID_TYPE = 1;

         // Standard Entity property keys (title is always set)
         static constexpr const char *TitleProperty = "title";
         static constexpr const char *LongDescProperty = "longDesc";
         static constexpr const char *ShortDescProperty = "shortDesc";

         // Entity callbacks are triggered by various operations and take
         // arbitrary data as an argument. If the callback returns true, it will
         // be removed after execution. If it returns false, the callback will
         // persist. This can be used to implement both one-time triggers and
         // conditional removal.
         typedef std::function<bool(std::any)> EntityCallback;

         // Valid types for a single entity property value
         typedef std::variant<size_t, int, double, bool, std::string> PropertyValue;

      private:

         // Custom messages that should be displayed for certain events that act
         // on or with the Entity
         Messages msgs;

         // Entity tags are labels that are either set or not set and are an
         // easy method of categorization
         std::unordered_set<std::string> tags;

         // meta data associated with the entity
         std::unordered_map<std::string, std::string> meta;

         // Entity properties like title, description, etc.
         std::unordered_map<std::string, PropertyValue> properties;

         // Maps entity properties to their validation functions (if they exist)
         std::unordered_map<
            std::string,
            std::function<int(PropertyValue)>
         > propertyValidators;

      protected:

         std::mutex mutex;

         // every kind of Entity that we are by virtue of inheritance
         std::list<enum EntityType> types;

         // Pointer to the Game that contains the Entity (set to nullptr when
         // the Entity is removed)
         Game *game;

         const std::string name;
         std::string className;

         // The Entity's Lua state
         std::shared_ptr<LuaState> L;

         // Event triggers
         std::unique_ptr<event::EventListener> triggers;

         // One or more callbacks that will be executed when various operations
         // occur on the Entity.
         std::unordered_map<
            std::string,
            std::vector<std::shared_ptr<EntityCallback>>
         > callbacks;

         // Output streams
         std::unique_ptr<Trogout> outStream;
         std::unique_ptr<Trogerr> errStream;

         // Ordinarily, the lifetime of an Entity is managed by an instance of
         // Game via std::shared_ptrs. However, if an Entity is created in a Lua
         // state and is never assigned to a Game, Lua will manage it instead.
         // This boolean flag lets me know whether or not to hold Lua's garbage
         // collector responsible for a particular instance.
         bool managedByLua = false;

         // Returns true if the property value is a size_t.
         inline int isPropertyValueSizet(PropertyValue v) {

            return 0 == v.index() ? PROPERTY_VALID : PROPERTY_INVALID_TYPE;
         }

         // Returns true if the property value is an int.
         inline int isPropertyValueInt(PropertyValue v) {

            return 1 == v.index() ? PROPERTY_VALID : PROPERTY_INVALID_TYPE;
         }

         // Returns true if the property value is a double.
         inline int isPropertyValueDouble(PropertyValue v) {

            return 2 == v.index() ? PROPERTY_VALID : PROPERTY_INVALID_TYPE;
         }

         // Returns true if the property value is a bool.
         inline int isPropertyValueBool(PropertyValue v) {

            return 3 == v.index() ? PROPERTY_VALID : PROPERTY_INVALID_TYPE;
         }

         // Returns true if the property value is a string.
         inline int isPropertyValueString(PropertyValue v) {

            return 4 == v.index() ? PROPERTY_VALID : PROPERTY_INVALID_TYPE;
         }

         /********************************************************************/

         /*
            Executes all callbacks for the specified operation. Callbacks take
            as input arbitrary data (callback should know what kind of data it
            is based on the operation performed) and return true if they should
            be removed after execution and false if they should persist.

            Input:
               Operation (std::string)
               Data to pass to the callback (std::any)

            Output:
               (none)
         */
         void executeCallback(std::string operation, std::any data);

         /*
            Maps a property to a validation function. If a validation function
            exists for a given property, setProperty() won't set the value
            unless the validator returns PROPERTY_VALID.

            Input:
               Key (std::string)
               Validator (std::function<int(PropertyValud)>)

            Output:
               (none)
         */
         inline void setPropertyValidator(
            std::string key,
            std::function<int(PropertyValue)> validator
         ) {

            propertyValidators[key] = validator;
         }

         /*
            Displays the short description of an Entity.  This may be
            overridden by Entity types that have a different display format.

            Input:
               Being doing the observing

            Output:
               (none)
         */
         virtual void displayShort(Being *observer);
         inline void displayShort(const std::shared_ptr<Being> &being) {

            displayShort(being.get());
         }

      public:

         // If no output channel is specified when output is sent to the
         // Entity's output stream, this is the channel we use
         static constexpr const char *DEFAULT_OUTPUT_CHANNEL = "notifications";

         /*
            Returns true if the given entity or class name is valid and false
            otherwise.

            Input:
               Entity or class name (std::string)

            Output:
               True if the name is valid and false if not
         */
         static inline bool isNameValid(std::string name) {

            // There are certain names that are forbidden to entities
            for (
               const char **reservedNamePtr = reservedNames;
               *reservedNamePtr != nullptr;
               reservedNamePtr++
            ) {
               if (0 == name.compare(*reservedNamePtr)) {
                  return false;
               }
            }

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
               {"resource", ENTITY_RESOURCE},
               {"tangible", ENTITY_TANGIBLE},
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
               {ENTITY_RESOURCE, "resource"},
               {ENTITY_TANGIBLE, "tangible"},
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
               Pointer to error stream object (Trogerr *)
         */
         Entity(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

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
               Callback (std::shared_ptr<EntityCallback>)

            Output:
               (none)
         */
         void addCallback(
            std::string operation,
            std::shared_ptr<EntityCallback> callback
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
               The specific callback to remove (const std::shared_ptr<EntityCallback> &)

            Output:
               (none)
         */
         void removeCallback(
            std::string operation,
            const std::shared_ptr<EntityCallback> &callback
         );

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
            Returns true if the entity property is set and false if not.

            Input:
               Key (std::string)

            Output:
               Whether or not the property is set (bool)
         */
         inline bool isPropertySet(std::string key) const {

            return properties.end() != properties.find(key) ? true : false;
         }

         /*
            Returns the value of a property. Throws std::invalid_argument if the
            property isn't set and std::bad_variant_access if an attempt is made
            to access a property with the incorrect type.

            Template arguments:
               The type to be returned

            Input:
               Key (std::string)

            Output:
               Property (template type)
         */
         template<typename T> inline const T getProperty(std::string key) const {

            if (properties.end() == properties.find(key)) {
               throw std::invalid_argument(std::string("attempted to access undefined entity property '") + key + "'");
            }

            return std::get<T>(properties.find(key)->second);
         }

         /*
            Sets a property if validation is successful and returns
            PROPERTY_VALID. If validation fails, another integer status code is
            returned indicating the nature of the failure (validators can use
            their own defined constants, but should respect those that have
            already been defined by the Entity class.)

            If validation passes and the property is set, any setProperty
            callbacks will be executed after the fact, with the Entity, property
            key, and property value being passed in as a std::tuple.

            Input:
               Key (std::string)
               Value (PropertyValue)

            Output:
               A status code indicating success or the reason for failure (int)
         */
         inline int setProperty(std::string key, PropertyValue value) {

            int status = PROPERTY_VALID;

            if (propertyValidators.end() != propertyValidators.find(key)) {
               status = propertyValidators[key](value);
            }

            if (PROPERTY_VALID == status) {

               mutex.lock();
               properties[key] = value;
               mutex.unlock();

               executeCallback(
                  "setProperty",
                  std::tuple<Entity *, std::string, PropertyValue>({this, key, value})
               );
            }

            return status;
         }

         // Without wrapping around const char *, we won't properly detect and
         // set bare C-style strings. WARNING: passing in a literal 0 to
         // setProperty() without casting (example: setProperty("key", 0) results
         // in the result being seen as a nullptr and triggers this wrapper
         // method as if it were a string, which will throw an exception. Grr.)
         inline int setProperty(std::string key, const char *value) {

            return setProperty(key, std::string(value));
         }

         /*
            Removes a property and returns the number of elements removed
            (effectively 1 if the element existed and 0 if it didn't.)

            Input:
               Key (std::string)

            Output:
               Number of elements erased (size_t)
         */
         inline size_t removeProperty(std::string key) {

            mutex.lock();
            size_t numErased = properties.erase(key);
            mutex.unlock();

            return numErased;
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
            Static method that takes as input iterators over a list of Entities
            that match a given name or alias, asks the Player for clarification
            if there's more than one and returns the chosen Entity.

            Template Arguments:
               ContainerType  -- iterable container type that contains entities

            Input:
               Iterators over entities to choose from
               Pointer to Entity representing the user (probably a Player)

            Output:
               (none)
         */
         template <typename ContainerType>
         static void clarifyEntity(ContainerType items, Entity *user);
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

   typedef std::list<Resource *> ResourceList;
   typedef std::list<Tangible *> TangibleList;
   typedef std::list<Place *>    PlaceList;
   typedef std::list<Room *>     RoomList;
   typedef std::list<Thing *>    ThingList;
   typedef std::list<Being *>    BeingList;
   typedef std::list<Player *>   PlayerList;
   typedef std::list<Creature *> CreatureList;
   typedef std::list<Object *>   ObjectList;

   typedef std::unordered_map<std::string, ResourceList> ResourcesByNameMap;
   typedef std::unordered_map<std::string, TangibleList> TangiblesByNameMap;
   typedef std::unordered_map<std::string, ThingList>    ThingsByNameMap;
   typedef std::unordered_map<std::string, BeingList>    BeingsByNameMap;
   typedef std::unordered_map<std::string, PlayerList>   PlayersByNameMap;
   typedef std::unordered_map<std::string, CreatureList> CreaturesByNameMap;
   typedef std::unordered_map<std::string, ObjectList>   ObjectsByNameMap;

   /***************************************************************************/

   // Special empty lists used for returning results when no result exists
   extern ResourceList  emptyResourceList;
   extern TangibleList  emptyTangibleList;
   extern PlaceList     emptyPlaceList;
   extern RoomList      emptyRoomList;
   extern ThingList     emptyThingList;
   extern BeingList     emptyBeingList;
   extern PlayerList    emptyPlayerList;
   extern CreatureList  emptyCreatureList;
   extern ObjectList    emptyObjectList;

   /***************************************************************************/

   // For the sake of simplifying my code, I make the assumption that there are
   // at least two entites in the collection and that this is why clarifyEntity
   // was called. Unless you enjoy undefined behavior, I suggest you only call
   // this on such a collection.
   template <typename ContainerType>
   void Entity::clarifyEntity(ContainerType objects, Entity *user) {

      user->out() << "Do you mean the ";

      // This loop is a little weird.  The logic was nicer when I was using a
      // list and could get its size, but I had to hack and clobber the existing
      // loop to make it work with iterators for cases where I don't know the
      // number of elements.
      for (auto i = objects.begin(); i != objects.end(); ) {

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
   }
}


#endif
