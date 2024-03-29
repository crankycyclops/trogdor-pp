#ifndef GAME_H
#define GAME_H


#include <any>
#include <memory>
#include <iostream>
#include <functional>
#include <sstream>
#include <string>
#include <cstdlib>
#include <optional>

#include <thread>
#include <mutex>

#include <trogdor/vocabulary.h>
#include <trogdor/command.h>
#include <trogdor/event/eventhandler.h>
#include <trogdor/event/eventlistener.h>
#include <trogdor/instantiator/instantiators/runtime.h>
#include <trogdor/serial/serializable.h>

#include <trogdor/iostream/trogout.h>
#include <trogdor/iostream/trogerr.h>

#include <trogdor/exception/entityexception.h>
#include <trogdor/exception/undefinedexception.h>


namespace trogdor {


   // Resolve circular dependencies
   class Action;
   class Parser;
   class Timer;
   class TimerJob;
   class LuaState;

   namespace entity {

      // Resolve circular dependencies
      class Entity;
      class Resource;
      class Tangible;
      class Place;
      class Thing;
      class Being;
      class Room;
      class Object;
      class Player;
      class Creature;
   };


   /*
      Each Game object represents a self contained game and contains all
      game-related data.  A standard lifecycle of a game object--assuming it
      exists in main()--might look something like:

      // errStream's type is std::unique_ptr<StreamOut>
      Game currentGame = std::make_unique<Game>(errStream);

      currentGame->initialize(Parser *parser, std::string gameFile);
      currentGame->start();

      while (currentGame->inProgress()) {
         ...
         read/execute a command;
         ...
      }

      return EXIT_SUCCESS;
   */
   class Game {

      public:

         static const bool DEFAULT_INTRODUCTION_ENABLED = false;
         static const bool DEFAULT_INTRODUCTION_PAUSE   = false;

         // Game callbacks are executed during various operations on the Game
         // object. Returning true will cause the callback to removed when it's
         // done executing and false will allow it to persist.
         typedef std::function<bool(std::any)> GameCallback;

      private:

         // Whether or not a game is in progress
         bool inGame;

         // Lock on this to keep data consistent between threads. I had a good
         // reason to use the recursive_mutex at one time, but now that the code
         // has changed significantly I may not need it anymore. I need to
         // revisit this.
         std::recursive_mutex mutex;

         // Keeps time in the game and executes scheduled jobs
         std::unique_ptr<Timer> timer;

         // Game meta data (like title, description, etc.)
         std::unordered_map<std::string, std::string> meta;

         // Maintains the game's vocabulary
         Vocabulary vocabulary;

         // Used to call subscribed event listeners
         event::EventHandler events;

         // Global EventListener for the entire game
         std::unique_ptr<event::EventListener> eventListener;

         // Global Lua state for the game
         std::shared_ptr<LuaState> L;

         // Defines if and how a player is presented with an introduction when
         // they're first added to the game
         struct {
            bool enabled;            // whether to show new players an intro
            std::string text;        // introduction's content
         } introduction;

         // Global error stream
         std::unique_ptr<Trogerr> errStream;

         // One or more callbacks that will be executed when various operations
         // occur within the game.
         std::unordered_map<
            std::string,
            std::vector<std::shared_ptr<GameCallback>>
         > callbacks;

         // Player object representing default settings for all new players
         std::unique_ptr<entity::Player> defaultPlayer;

         // Hash table of all entities in the game
         std::unordered_map<std::string, std::shared_ptr<entity::Entity>> entities;
         std::unordered_map<std::string, std::shared_ptr<entity::Resource>> resources;
         std::unordered_map<std::string, std::shared_ptr<entity::Tangible>> tangibles;
         std::unordered_map<std::string, std::shared_ptr<entity::Place>> places;
         std::unordered_map<std::string, std::shared_ptr<entity::Thing>> things;
         std::unordered_map<std::string, std::shared_ptr<entity::Room>> rooms;
         std::unordered_map<std::string, std::shared_ptr<entity::Being>> beings;
         std::unordered_map<std::string, std::shared_ptr<entity::Player>> players;
         std::unordered_map<std::string, std::shared_ptr<entity::Creature>> creatures;
         std::unordered_map<std::string, std::shared_ptr<entity::Object>> objects;

         /*
            Called by initialize().  This initializes event handling in the game.

            Input: (none)
            Output: (none)
         */
         void initEvents();

         /*
            Does the actual work of deserialization. Called by both deserialize()
            and the deserialization constructor.

            Input:
               Serialized data (std::shared_ptr<serial::Serializable>)
               Callback that generates each player's output stream
               Callback that generates each player's error stream (optional)

            Output:
               (none)
         */
         void _deserialize(
            std::shared_ptr<serial::Serializable> data,
            std::function<std::unique_ptr<Trogout>(Game *)> makeOutStream,
            std::function<std::unique_ptr<Trogerr>(Game *)> makeErrStream = {}
         );

      public:

         /*
            Constructor for the Game class (require the error stream as an
            argument.) The second constuctor creates an instance of Game from
            serialized data (for an explanation of the arguments, see the
            definition of Game::deserialize.)
         */
         Game() = delete;

         Game(
            std::unique_ptr<Trogerr> e,
            std::optional<size_t> timerTickInterval = std::nullopt
         );

         Game(
            std::shared_ptr<serial::Serializable> data,
            std::unique_ptr<Trogerr> gameErrStream,
            std::function<std::unique_ptr<Trogout>(Game *)> makeOutStream,
            std::function<std::unique_ptr<Trogerr>(Game *)> makeErrStream = {}
         );

         /*
            Don't allow copying since I don't currently have a good reason to
            clone a Game object and won't take the time to write that
            functionality.
         */
         Game &operator=(const Game &) = delete;
         Game(const Game &) = delete;

         /*
            Empty destructor defined in game.cpp. If this isn't defined, the
            compiler will choke (God knows why.) In unique_ptr.h, I get this
            error message from GCC: error: invalid application of ‘sizeof’ to
            incomplete type 'trogdor::Timer'.
         */
         ~Game();

         /*
            Returns an easily serializable version of a Game instance.

            Input:
               (none)

            Output:
               A version of Game in an easily serializable format
         */
         std::shared_ptr<serial::Serializable> serialize();

         /*
            Replaces the current game state with a previously serialized object.
            Takes as input callbacks that create output and error streams for
            each deserialized player. The error stream callback is optional, and
            if the argument isn't provided, the Game object's own error stream
            will be used.

            Input:
               Serialized data (std::shared_ptr<serial::Serializable>)
               Callback that generates each player's output stream
               Callback that generates each player's error stream (optional)

            Output:
               (none)
         */
         void deserialize(
            std::shared_ptr<serial::Serializable> data,
            std::function<std::unique_ptr<Trogout>(Game *)> makeOutStream,
            std::function<std::unique_ptr<Trogerr>(Game *)> makeErrStream = {}
         );

         /*
            Returns an instance of the Runtime instantiator.

            Input:
               (none)

            Output:
               New instance of Runtime instantiator (std::unique_ptr<Runtime>)
         */
         std::unique_ptr<Runtime> makeInstantiator();

         /*
            Returns a reference to Game instance's LuaState object.

            I'm returning a const reference to a shared_ptr per this advice:
            "Use a const shared_ptr& as a parameter only if you're not sure
            whether or not you'll take a copy and share ownership," which I
            found here:
            https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/

            Input:
               (none)

            Output:
               Game's Lua State (const &LuaState)
         */
         const std::shared_ptr<LuaState> &getLuaState() {return L;}

         /*
            Returns a pointer to Game instance's EventListener.

            Input:
               (none)

            Output:
               Pointer to Game's EventListener (event::EventListener *)
         */
         event::EventListener *getEventListener() {return eventListener.get();}

         /*
            Returns a reference to the Entity's error stream.  A typical use
            would look something like this:

            game->err() << "I'm an error!" << std::endl;

            Or, with a severity other than ERROR:

            game->err(Trogerr::WARNING) << "I'm a warning!" << std::endl;

            Input:
               Error severity level (default: ERROR)

            Output:
               Trogerr &
         */
         Trogerr &err(Trogerr::ErrorLevel severity = Trogerr::ERROR) {

            errStream->setErrorLevel(severity);
            return *errStream;
         }

         /*
            Returns an iterable list of all meta pairs.

            Input:
               (none)

            Output:
               const std::unordered_map<std::string, std::string> const &
         */
         inline const std::unordered_map<std::string, std::string> &getMetaAll() const {return meta;}

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
            Sets a meta data value.

            Input:
               meta key (std::string)
               value (std::string)

            Output:
               (none)
         */
         inline void setMeta(std::string key, std::string value) {meta[key] = value;}

         /*
            Returns a const reference to the game's vocabulary for lookups.

            Input:
               (none)

            Output:
               const Vocabulary &
         */
         inline const Vocabulary &getVocabulary() const {

            return vocabulary;
         }

         /*
            Wraps around vocabulary.insertDirection. The reasoning behind this
            is that getVocabulary only returns a const reference for lookups to
            objects that aren't supposed to have much access to Game and
            therefore shouldn't be changing any of its values. By contrast, an
            object that has access to Game might have a good reason to modify
            its vocabulary.

            Input:
               New direction (std::string)

            Output:
               (none)
         */
         inline void insertDirection(std::string direction) {

            vocabulary.insertDirection(direction);
         }


         /*
            Wraps around vocabulary.insertDirectionSynonym. The reasoning behind
            this is that getVocabulary only returns a const reference for
            lookups to objects that aren't supposed to have much access to Game
            and therefore shouldn't be changing any of its values. By contrast,
            an object that has access to Game might have a good reason to modify
            its vocabulary.

            Input:
               New direction synonym (std::string)
               Actual direction synonym references (std::string)

            Output:
               (none)
         */
         inline void insertDirectionSynonym(std::string synonym, std::string direction) {

            vocabulary.insertDirectionSynonym(synonym, direction);
         }

         /*
            Returns whether or not new player introductions are enabled.

            Input:
               (none)

            Output:
               Whether or not new player introductions are enabled (bool)
         */
         inline bool getIntroductionEnabled() {return introduction.enabled;}

         /*
            Sets whether or not new player introductions are enabled.

            Input:
               Whether or not to enable intros (bool)

            Output:
               (none)
         */
         inline void setIntroductionEnabled(bool e) {introduction.enabled = e;}

         /*
            Sets the content for the new player introduction. Won't be displayed
            when a player is added to the game unless introduction.enabled is
            set to true.

            Input:
               Content for new player introductions (std::string)

            Output:
               (none)
         */
         inline void setIntroductionText(std::string t) {introduction.text = t;}

         /*
            Gets the default player. Used to initialize default settings for
            new players.

            Input:
               (none)

            Output:
               entity::Player *
         */
         inline entity::Player *getDefaultPlayer() {

            return defaultPlayer.get();
         }

         /*
            Returns whether or not a player by the specified name is in the
            game.

            Input:
               Player name (std::string)

            Output:
               True if the player is in the game and false if not
         */
         inline bool playerIsInGame(const std::string name) const {

            return players.find(name) == players.end() ? false : true;
         }

        /*
            Returns the Entity associated with the specified name.

            Input:
               Name of Thing (std::string)

            Output:
               const shared_ptr<Entity> &
         */
         inline const std::shared_ptr<entity::Entity> &getEntity(const std::string name) {

            static std::shared_ptr<entity::Entity> nullEntity(nullptr);

            if (entities.find(name) == entities.end()) {
               return nullEntity;
            }

            return entities[name];
         }

         /*
            Returns a read-only unordered_map of all entities
            (shared_ptr<Entity>) in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Entity>> &
         */
         inline const auto &getEntities() const {return entities;}

        /*
            Returns the Resoure associated with the specified name.

            Input:
               Name of Resource (std::string)

            Output:
               const shared_ptr<Resource> &
         */
         inline const std::shared_ptr<entity::Resource> &getResource(const std::string name) {

            static std::shared_ptr<entity::Resource> nullResource(nullptr);

            if (resources.find(name) == resources.end()) {
               return nullResource;
            }

            return resources[name];
         }

         /*
            Returns a read-only unordered_map of all resources
            (shared_ptr<Resource>) in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Resource>> &
         */
         inline const auto &getResources() const {return resources;}

        /*
            Returns the Tangible associated with the specified name.

            Input:
               Name of Tangible (std::string)

            Output:
               const shared_ptr<Tangible> &
         */
         inline const std::shared_ptr<entity::Tangible> &getTangible(const std::string name) {

            static std::shared_ptr<entity::Tangible> nullTangible(nullptr);

            if (tangibles.find(name) == tangibles.end()) {
               return nullTangible;
            }

            return tangibles[name];
         }

         /*
            Returns a read-only unordered_map of all tangibles
            (shared_ptr<Tangible>) in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Tangible>> &
         */
         inline const auto &getTangibles() const {return tangibles;}

        /*
            Returns the Place object associated with the specified name.

            Input:
               Name of Place (std::string)

            Output:
               Place *
         */
         inline const std::shared_ptr<entity::Place> &getPlace(const std::string name) {

            static std::shared_ptr<entity::Place> nullPlace(nullptr);

            if (places.find(name) == places.end()) {
               return nullPlace;
            }

            return places[name];
         }

         /*
            Returns a read-only unordered_map of all places (shared_ptr<Place>)
            in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Room>> &
         */
         inline const auto &getPlaces() const {return places;}

        /*
            Returns the Thing object associated with the specified name.

            Input:
               Name of Thing (std::string)

            Output:
               Thing *
         */
         inline const std::shared_ptr<entity::Thing> &getThing(const std::string name) {

            static std::shared_ptr<entity::Thing> nullThing(nullptr);

            if (things.find(name) == things.end()) {
               return nullThing;
            }

            return things[name];
         }

         /*
            Returns a read-only unordered_map of all things (shared_ptr<Thing>)
            in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Thing>> &
         */
         inline const auto &getThings() const {return things;}

         /*
            Returns the Being object associated with the specified being name.

            Input:
               Name of being (std::string)

            Output:
               Being *
         */
         inline const std::shared_ptr<entity::Being> &getBeing(const std::string name) {

            static std::shared_ptr<entity::Being> nullBeing(nullptr);

            if (beings.find(name) == beings.end()) {
               return nullBeing;
            }

            return beings[name];
         }

         /*
            Returns a read-only unordered_map of all beings (shared_ptr<Being>)
            in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Being>> &
         */
         inline const auto &getBeings() const {return beings;}

         /*
            Returns the Player object associated with the specified player name.

            Input:
               Name of player (std::string)

            Output:
               Player *
         */
         inline const std::shared_ptr<entity::Player> &getPlayer(const std::string name) {

            static std::shared_ptr<entity::Player> nullPlayer(nullptr);

            if (players.find(name) == players.end()) {
               return nullPlayer;
            }

            return players[name];
         }

         /*
            Returns a read-only unordered_map of all players (shared_ptr<Player>)
            in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Player>> &
         */
         inline const auto &getPlayers() const {return players;}

         /*
            Returns the Creature object associated with the specified name.

            Input:
               Name of creature (std::string)

            Output:
               Creature *
         */
         inline const std::shared_ptr<entity::Creature> &getCreature(const std::string name) {

            static std::shared_ptr<entity::Creature> nullCreature(nullptr);

            if (creatures.find(name) == creatures.end()) {
               return nullCreature;
            }

            return creatures[name];
         }

         /*
            Returns a read-only unordered_map of all creatures
            (shared_ptr<Creature>) in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Creature>> &
         */
         inline const auto &getCreatures() const {return creatures;}

         /*
            Returns the Object associated with the specified name.

            Input:
               Name of Object (std::string)

            Output:
               Object *
         */
         inline const std::shared_ptr<entity::Object> &getObject(const std::string name) {

            static std::shared_ptr<entity::Object> nullObject(nullptr);

            if (objects.find(name) == objects.end()) {
               return nullObject;
            }

            return objects[name];
         }

         /*
            Returns a read-only unordered_map of all objects (shared_ptr<Object>)
            in the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Object>> &
         */
         inline const auto &getObjects() const {return objects;}

         /*
            Returns the Room associated with the specified name.

            Input:
               Name of Room (std::string)

            Output:
               Room *
         */
         inline const std::shared_ptr<entity::Room> &getRoom(const std::string name) {

            static std::shared_ptr<entity::Room> nullRoom(nullptr);

            if (rooms.find(name) == rooms.end()) {
               return nullRoom;
            }

            return rooms[name];
         }

         /*
            Returns a read-only unordered_map of all rooms (shared_ptr<Room>) in
            the game.

            Input:
               (none)

            Output:
               const unordered_map<string, shared_ptr<Room>> &
         */
         inline const auto &getRooms() const {return rooms;}

         /*
            Inserts an entity into the game.

            Input:
               shared_ptr<entity::Entity>

            Output:
               (none)
         */
         void insertEntity(std::string name, std::shared_ptr<entity::Entity> entity);

         /*
            Removes the Entity referenced by name from the game and throws an
            instance of EntityException or UndefinedException.

            Removal of an Entity will only be successful under the following
            conditions:

               1) The Entity is not an Object in a Being's inventory; AND
               2) The Entity is not a Resource held by a Room or a Being; AND
               3) The Entity is not contained inside a Place; AND
               4) The Entity is not a Place that contains one or more Things; AND
               5) The Entity is not a Being that owns one or more Objects

            My logic for requiring these conditions to be met before an Entity
            can be removed is that it forces the developer to decide what to do
            before Entities that are owned by other Entities (or the other way
            around) are removed. For example, if an Object that's a part of a
            Player's inventory is removed, the developer might want to send them
            a message about it.

            To prepare the Entity for removal, you'll want to do something like
            the following:

            // In the case of a Thing, you'll have to make sure it's not inside
            // a Place:
            if (auto place = thing->getLocation().lock()) {
               // Do something before removing it from its current location
               thing->getLocation()->removeThing(thing);
            }

            // In the case of an Object, you'll also want an additional check
            // for ownership by a Being
            if (auto owner = object->getOwner().lock()) {
               // Do whatever you want before removing the object from the inventory
               owner->removeItemFromInventory(object);
            }

            // In the case of a Resource, check to see if it has any allocations
            for (auto const &depositor: resource->getDepositors()) {
               // Do something (like warn a player, maybe) first
               resource->free(depositor);
            }

            // In the case of a Place, check to see if it contains any Things
            for (auto const &thing: resource->getDepositors()) {
               // Do something first if necessary
               place->removeThing(thing);
            }

            // In the case of a Being, make sure it doesn't own anything
            for (auto const &objPtr: being->getInventoryObjects()) {
               if (auto const &object = objPtr.lock()) {
                  // Do something first if necessary
                  being->removeFromInventory(object);
               }
            }

            When an entity is removed from the game, it will continue to live
            until the last shared_ptr referencing it falls out of scope. In
            order to signify that such an Entity has been removed and is no
            longer to be considered a part of the game, its Game pointer will be
            set to nullptr. In order to check that an Entity is still valid
            before working with it, do something like the following:

            // Instance of Entity will be considered removed and thus invalid if
            // entity->getGame() returns nullptr
            if (entity->getGame()) {
               // Do whatever you were going to do because the Entity is valid
            }

            Input:
               shared_ptr<entity::Entity>

            Output:
               (none)
         */
         void removeEntity(std::string name);

         /*
            Removes all entities from the game.

            Input:
               (none)

            Output:
               (none)
         */
         inline void clearEntities() {

            entities.clear();
            resources.clear();
            tangibles.clear();
            places.clear();
            rooms.clear();
            things.clear();
            objects.clear();
            beings.clear();
            creatures.clear();
            players.clear();
         }

         /*
            Initializes the game, including the event handler, timer and all game
            entities.

            Input:
               Pointer to an instance of Parser (Parser *)
               Game definition filename (std::string)
               Whether or not exceptions should be handled by this class (default: false)

            Output:
               True if initialization was successful and false if not (bool)
         */
         bool initialize(Parser *parser, std::string gamefile, bool handleExceptions = false);

         /*
            Puts the game into a running state. In a running state, the timer is
            ticking, and player commands are executed.

            To pause and resume a game, simply stop and start it.

            If one or more callbacks have been added with the key "start", they
            will be called after the game is started with no arguments.

            Input: (none)
            Output: (none)
         */
         void start();

         /*
            Puts the game into a stopped state. In a stopped state, the timer is
            not running (though it retains the current time), and player commands
            are not executed.

            To pause and resume a game, simply stop and start it.

            If one or more callbacks have been added with the key "stop", they
            will be called after the game is started with no arguments.

            Input: (none)
            Output: (none)
         */
         void stop();

         /*
            Deactivates the game and timer without calling Timer::stop(), which
            waits to join on the timer thread. The only reason I added this
            method and made it public was to enable an optimization where you
            need to stop a large number of games at once and it would take too
            long to stop each timer serially. You should NOT call this method
            unless you have a good reason to.

            If you have to use this, it should always be followed by a call to
            shutdown, like so:

            game->deactivate();
            ... any necessary cleanup code ...
            game->shutdown();

            Input: (none)
            Output: (none)
         */
         void deactivate();

         /*
            Stops the game and timer after a call to deactivate(). Should never
            be called until after a previous call to deactivate(). The only
            reason I added this method and made it public was to enable an
            optimization where you need to stop a large number of games at once
            and it would take too long to stop each timer serially. You should
            NOT call this method unless you have a good reason to.

            If you have to use this, it should be done in the following
            order:

            game->deactivate();
            ... any necessary cleanup code ...
            game->shutdown();

            Input: (none)
            Output: (none)
         */
         void shutdown();

         /*
            Returns the status of the game.

            Input: (none)
            Output: boolean true if the game is running and false if it's not.
         */
         inline bool inProgress() const {return inGame;}

         /*
            Creates a new player (but does not insert it into the game.) Throws
            an exception if an entity with the given name already exists in the
            game.

            Input:
               Player name (std::string)
               Pointer to an output stream (Trogout *)
               Pointer to an error stream (Trogerr *)

            Output:
               std::shared_ptr<Player>
         */
         std::shared_ptr<entity::Player> createPlayer(std::string name,
         std::unique_ptr<Trogout> outStream, std::unique_ptr<Trogerr> errStream);

         /*
            Inserts a player into the game. Throws an exception if an entity
            with the given name already exists in the game.

            If a callback is provided and new player introductions are enabled
            via game.xml, it will be called after the introduction is displayed
            and before the player is actually inserted into the game. Its
            primary purpose is to give the client the option of confirming that
            the user has read the introduction before they can start playing.

            If one or more callbacks have been set on the Game object with the
            key "beforeInsertPlayer", it will be called before the player has
            been inserted with the player as the only argument. If one or more
            callbacks have been set with the key "afterInsertPlayer", the same
            will occur after all player-insertion logic has executed. 

            If we're deserializing a pre-existing game, the introduction will
            not be shown to the user, the callback will be ignored, and the
            player's starting location will not be set to "start."

            Input:
               Player name (std::string)
               Callback to execute after player has been inserted (std::function, optional)
               Whether or not we're inserting the player as part of Game::deserialize (bool)

            Output:
               Player *
         */
         void insertPlayer(
            std::shared_ptr<entity::Player> player,
            std::function<void()> confirmationCallback = nullptr,
            bool deserialize = false
         );

         /*
            Removes a player from the game.  Does nothing if the player with the
            specified name doesn't exist.

            If one or more callbacks have been set on the Game object with the
            key "removePlayer", it will be called before the player has been
            removed with the player as the only argument.

            Input:
               Name of player (std::string)
               Message to output to player before removing (std::string: default is none)

            Output:
               (none)
         */
         void removePlayer(
            const std::string name,
            const std::string message = ""
         );

         /*
            Wraps around Timer API.  See timer.h for documentation.
         */
         void insertTimerJob(std::shared_ptr<TimerJob> j);
         void removeTimerJob(std::shared_ptr<TimerJob> j);
         void setTickInterval(size_t period);

         /*
            Gets the current game time (in seconds.)  Note that I can't inline
            this due to forward declaration stuff.  F*#@ me!

            Input: (none)
            Output: time in seconds (unsigned long)
         */
         unsigned long getTime() const;

         /*
            Wraps around Vocabulary::insertVerbAction, allowing the client to
            supply its own custom actions. See vocabulary.h for documentation.
         */
         void insertVerbAction(std::string verb, std::unique_ptr<Action> action);

         /*
            Wraps around Vocabulary::insertSynonym, allowing the client to
            supply its own verb synonyms. See vocabulary.h for documentation.
         */
         inline void insertVerbSynonym(std::string synonym, std::string verb) {

            vocabulary.insertVerbSynonym(synonym, verb);
         }

         /*
            Wraps around EventHandler API.  See eventhandler.h for documentation.
         */
         inline bool event(event::Event e) {

            // make sure global EventListener is always listening
            e.prependListener(eventListener.get());
            return events.dispatch(e);
         }

         /*
            Execute an action on a player's behalf. Returns true if the action
            was found and the command syntax was valid and false if not.

            Input:
               Player issuing the command (entity::Player *)
               The player's command (const Command &)

            Output:
               True if the action was found and the syntax was valid and false
               if not.
         */
         bool executeAction(entity::Player *player, const Command &command);

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
            Adds a callback that should be called when a certain operation
            occurs in the game. A callback that returns true will remove itself
            after execution and a callback that returns false will persist.

            Input:
               Operation the callback should be attached to (std::string)
               Callback (std::shared_ptr<GameCallback>)

            Output:
               (none)
         */
         void addCallback(
            std::string operation,
            std::shared_ptr<GameCallback> callback
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
            const std::shared_ptr<GameCallback> &callback
         );
   };
}


#endif
