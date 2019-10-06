#ifndef GAME_H
#define GAME_H


#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <thread>
#include <mutex>

#include "vocabulary.h"
#include "command.h"
#include "event/eventhandler.h"
#include "entitymap.h"
#include "iostream/trogout.h"
#include "instantiator/instantiators/runtime.h"

#include "exception/entityexception.h"
#include "exception/undefinedexception.h"


using namespace std;

namespace trogdor {


   // Resolve circular dependencies
   class Action;
   class Parser;
   class Timer;
   class TimerJob;


   /*
      Each Game object represents a self contained game and contains all
      game-related data.  A standard lifecycle of a game object--assuming it
      exists in main()--might look something like:

      // errStream's type = std::unique_ptr<StreamOut>
      Game currentGame = std::make_unique<Game>(errStream);

      currentGame->initialize(Parser *parser, string gameFile);
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

         /* lock on this to keep data consistent between threads */
         std::mutex resourceMutex;

         /* lock on this to synchronize timer actions */
         std::mutex timerMutex;

      private:

         // whether or not a game is in progress
         bool inGame;

         // Keeps time in the game and executes scheduled jobs
         std::unique_ptr<Timer> timer;

         // the last executed command
         std::shared_ptr<Command> lastCommand;

         // game meta data (like title, description, etc.)
         unordered_map<string, string> meta;

         // Maintains the game's vocabulary
         Vocabulary vocabulary;

         // used to call subscribed event listeners
         std::unique_ptr<event::EventHandler> events;

         // Global Lua state for the entire game
         std::shared_ptr<LuaState> L;

         // Global EventListener for the entire game
         std::unique_ptr<event::EventListener> eventListener;

         // Player object representing default settings for all new players
         std::unique_ptr<entity::Player> defaultPlayer;

         // Hash table of all entities in the game
         // Note: the logical conclusion of having a hierarchical mapping of
         // object types is that no object of any type can share the same name!
         // This can be worked around via aliases :)
         entity::EntityMap    entities;
         entity::PlaceMap     places;
         entity::ThingMap     things;
         entity::RoomMap      rooms;
         entity::BeingMap     beings;
         entity::PlayerMap    players;
         entity::CreatureMap  creatures;
         entity::ObjectMap    objects;

         // defines if and how a player is presented with an introduction when
         // they're first added to the game
         struct {
            bool enabled;            // whether to show new players an intro
            bool pauseWhileReading;  // whether to pause the game during the intro
            string text;             // introduction's content
         } introduction;

         /* global error stream */
         std::unique_ptr<Trogout> errStream;

         /*
            Called by initialize().  This initializes event handling in the game.

            Input: (none)
            Output: (none)
         */
         void initEvents();

      public:

         /*
            Constructor for the Game class (require the error stream as an
            argument.)
         */
         Game() = delete;
         Game(std::unique_ptr<Trogout> e);

         /*
            Don't allow copying since I don't currently have a good reason to
            clone a Game object and won't take the time to write that
            functionality.
         */
         Game& operator=(const Game &) = delete;
         Game(const Game &) = delete;

         /*
            Empty destructor defined in game.cpp. If this isn't defined, the
            compiler will choke (God knows why.) In unique_ptr.h, I get this
            error message from GCC: error: invalid application of ‘sizeof’ to
            incomplete type 'trogdor::Timer'.
         */
         ~Game();

         /*
            Returns an instance of the Runtime instantiator.

            Input:
               (none)

            Output:
               New instance of Runtime instantiator (std::unique_ptr<Runtime>)
         */
         inline std::unique_ptr<Runtime> makeInstantiator() {

            return make_unique<Runtime>(getVocabulary(), this);
         }

         /*
            Returns a reference to Game instance's LuaState object. This should
            ONLY be used by the instantiator and (possibly) other select classes
            that need to interact directly with this. THIS IS A DANGEROUS METHOD.
            You have been warned.

            Input:
               (none)

            Output:
               Game's Lua State (const &LuaState)
         */
         std::shared_ptr<LuaState> &getLuaState() {return L;}

         /*
            Returns a reference to Game instance's EventListener.

            Input:
               (none)

            Output:
               Game's EventListener (const &event::EventListener)
         */
         std::unique_ptr<event::EventListener> &getEventListener() {return eventListener;}

         /*
            Returns a reference to the Game's error stream.  A typical use
            would look something like this:

            gamePtr->err() << "I'm an error message!" << endl;

            Input:
               (none)

            Output:
               Trogout &
         */
         Trogout &err() {return *errStream;}

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
            Sets a meta data value.

            Input:
               meta key (string)
               value (string)

            Output:
               (none)
         */
         inline void setMeta(string key, string value) {meta[key] = value;}

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
               New direction (string)

            Output:
               (none)
         */
         inline void insertDirection(string direction) {

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
               New direction synonym (string)
               Actual direction synonym references (string)

            Output:
               (none)
         */
         inline void insertDirectionSynonym(string synonym, string direction) {

            vocabulary.insertDirectionSynonym(synonym, direction);
         }

         /*
            Sets whether or not new player introductions are enabled.

            Input:
               Whether or not to enable intros (bool)

            Output:
               (none)
         */
         inline void setIntroductionEnabled(bool e) {introduction.enabled = e;}

         /*
            Sets whether or not to pause the game while a new player reads the
            introduction. This is primarily for single player games.

            Input:
               Whether or not to pause while reading (bool)

            Output:
               (none)
         */
         inline void setIntroductionPause(bool p) {introduction.pauseWhileReading = p;}

         /*
            Sets the content for the new player introduction. Won't be displayed
            when a player is added to the game unless introduction.enabled is
            set to true.

            Input:
               Content for new player introductions (string)

            Output:
               (none)
         */
         inline void setIntroductionText(string t) {introduction.text = t;}

         /*
            Gets the default player. Used to initialize default settings for
            new players.

            Input:
               (none)

            Output:
               Reference to unique_ptr<entity::Player>
         */
         inline std::unique_ptr<entity::Player> &getDefaultPlayer() {

            return defaultPlayer;
         }

         /*
            Removes a player from the game.  Does nothing if the player with the
            specified name doesn't exist.

            Input:
               Name of player (string)
               Message to output to player before removing (string: default is none)

            Output:
               (none)
         */
         inline void removePlayer(const string name, const string message = "") {

            if (players.isset(name)) {

               if (message.length()) {
                  players.get(name)->out("notifications") << message << endl;
               }

               entities.erase(name);
               things.erase(name);
               beings.erase(name);
               players.erase(name);
            }
         }

         /*
            Returns whether or not a player by the specified name is in the
            game.

            Input:
               Player name (string)

            Output:
               True if the player is in the game and false if not
         */
         inline bool playerIsInGame(const string name) const {

            return players.isset(name);
         }

        /*
            Returns the Entity associated with the specified name. Throws an
            exception if the Entity doesn't exist.

            Input:
               Name of Thing (string)

            Output:
               Thing *
         */
         inline Entity *getEntity(const string name) {

            if (!entities.isset(name)) {
               return nullptr;
            }

            return entities.get(name);
         }

         /*
            Returns a pair of iterators covering all Entities in the game.

            Input:
               (none)

            Output:
               Entity iterator
         */
         inline auto const entitiesBegin() {return entities.begin();}
         inline auto const entitiesEnd() {return entities.end();}

        /*
            Returns the Place object associated with the specified name. Throws
            an exception if the Place doesn't exist.

            Input:
               Name of Place (string)

            Output:
               Place *
         */
         inline Place *getPlace(const string name) {

            if (!things.isset(name)) {
               return nullptr;
            }

            return places.get(name);
         }

         /*
            Returns a pair of iterators covering all Places in the game. The
            underline pointers are of type shared_ptr<Entity> and must be cast
            to shared_ptr<Place> when accessing Place-specific methods.

            Input:
               (none)

            Output:
               Places iterator
         */
         inline auto const placesBegin() {return places.begin();}
         inline auto const placesEnd() {return places.end();}

        /*
            Returns the Thing object associated with the specified name. Throws
            an exception if the Thing doesn't exist.

            Input:
               Name of Thing (string)

            Output:
               Thing *
         */
         inline Thing *getThing(const string name) {

            if (!things.isset(name)) {
               return nullptr;
            }

            return things.get(name);
         }

         /*
            Returns a pair of iterators covering all Things in the game. The
            underline pointers are of type shared_ptr<Entity> and must be cast
            to shared_ptr<Thing> when accessing Thing-specific methods.

            Input:
               (none)

            Output:
               Things iterator
         */
         inline auto const thingsBegin() {return things.begin();}
         inline auto const thingsEnd() {return things.end();}

         /*
            Returns the Being object associated with the specified being name.
            Throws an exception if the specified being name doesn't exist.

            Input:
               Name of being (string)

            Output:
               Being *
         */
         inline Being *getBeing(const string name) {

            if (!beings.isset(name)) {
               return nullptr;
            }

            return beings.get(name);
         }

         /*
            Returns a pair of iterators covering all Beings in the game. The
            underline pointers are of type shared_ptr<Entity> and must be cast
            to shared_ptr<Being> when accessing Being-specific methods.

            Input:
               (none)

            Output:
               Beings iterator
         */
         inline auto const beingsBegin() {return beings.begin();}
         inline auto const beingsEnd() {return beings.end();}

         /*
            Returns the Player object associated with the specified player name.
            Throws an exception if the specified player name doesn't exist.

            Input:
               Name of player (string)

            Output:
               Player *
         */
         inline Player *getPlayer(const string name) {

            if (!players.isset(name)) {
               return nullptr;
            }

            return players.get(name);
         }

         /*
            Returns a pair of iterators covering all Players in the game. The
            underline pointers are of type shared_ptr<Entity> and must be cast
            to shared_ptr<Player> when accessing Player-specific methods.

            Input:
               (none)

            Output:
               Players iterator
         */
         inline auto const playersBegin() {return players.begin();}
         inline auto const playersEnd() {return players.end();}

         /*
            Returns the Creature object associated with the specified name.
            Throws an exception if the creature doesn't exist.

            Input:
               Name of creature (string)

            Output:
               Creature *
         */
         inline Creature *getCreature(const string name) {

            if (!creatures.isset(name)) {
               return nullptr;
            }

            return creatures.get(name);
         }

         /*
            Returns a pair of iterators covering all Creatures in the game. The
            underline pointers are of type shared_ptr<Entity> and must be cast
            to shared_ptr<Creature> when accessing Creature-specific methods.

            Input:
               (none)

            Output:
               Creatures iterator
         */
         inline auto const creaturesBegin() {return creatures.begin();}
         inline auto const creaturesEnd() {return creatures.end();}

         /*
            Returns the Object associated with the specified name. Throws an
            exception if the Object doesn't exist.

            Input:
               Name of Object (string)

            Output:
               Object *
         */
         inline Object *getObject(const string name) {

            if (!objects.isset(name)) {
               return nullptr;
            }

            return objects.get(name);
         }

         /*
            Returns a pair of iterators covering all Objects in the game. The
            underline pointers are of type shared_ptr<Entity> and must be cast
            to shared_ptr<Object> when accessing Object-specific methods.

            Input:
               (none)

            Output:
               Objects iterator
         */
         inline auto const objectsBegin() {return objects.begin();}
         inline auto const objectsEnd() {return objects.end();}

         /*
            Returns the Room associated with the specified name. Throws an
            exception if the Room doesn't exist.

            Input:
               Name of Room (string)

            Output:
               Room *
         */
         inline Room *getRoom(const string name) {

            if (!rooms.isset(name)) {
               return nullptr;
            }

            return rooms.get(name);
         }

         /*
            Returns a pair of iterators covering all Rooms in the game. The
            underline pointers are of type shared_ptr<Entity> and must be cast
            to shared_ptr<Room> when accessing Room-specific methods.

            Input:
               (none)

            Output:
               Rooms iterator
         */
         inline auto const roomsBegin() {return rooms.begin();}
         inline auto const roomsEnd() {return rooms.end();}

         /*
            Wraps around the other insertEntity methods in cases where I have a
            base class shared_ptr.

            Input:
               shared_ptr<entity::Entity>

            Output:
               (none)
         */
         inline void insertEntity(string name, std::shared_ptr<entity::Entity> entity) {

            switch (entity->getType()) {

               case entity::ENTITY_ROOM:
                  insertEntity(name, std::dynamic_pointer_cast<entity::Room>(entity));
                  break;

               case entity::ENTITY_OBJECT:
                  insertEntity(name, std::dynamic_pointer_cast<entity::Object>(entity));
                  break;

               case entity::ENTITY_CREATURE:
                  insertEntity(name, std::dynamic_pointer_cast<entity::Creature>(entity));
                  break;

               case entity::ENTITY_PLAYER:
                  insertEntity(name, std::dynamic_pointer_cast<entity::Player>(entity));
                  break;

               default:
                  throw UndefinedException("Game::insertEntity: unsupported entity type");
            }

            entity->setGame(this);
         }

         /*
            Inserts a player into the game.

            Input:
               shared_ptr<entity::Player>

            Output:
               (none)
         */
         inline void insertEntity(string name, std::shared_ptr<entity::Player> player) {

            if (entities.isset(name)) {
               throw entity::EntityException(string("Entity '") + name + "' already exists");
            }

            entities.set(name, player);
            things.set(name, player);
            beings.set(name, player);
            players.set(name, player);
         }

         /*
            Inserts a creature into the game.

            Input:
               shared_ptr<entity::Creature>

            Output:
               (none)
         */
         inline void insertEntity(string name, std::shared_ptr<entity::Creature> creature) {

            if (entities.isset(name)) {
               throw entity::EntityException(string("Entity '") + name + "' already exists");
            }

            entities.set(name, creature);
            things.set(name, creature);
            beings.set(name, creature);
            creatures.set(name, creature);
         }

         /*
            Inserts an object into the game.

            Input:
               shared_ptr<entity::Object>

            Output:
               (none)
         */
         inline void insertEntity(string name, std::shared_ptr<entity::Object> object) {

            if (entities.isset(name)) {
               throw entity::EntityException(string("Entity '") + name + "' already exists");
            }

            entities.set(name, object);
            things.set(name, object);
            objects.set(name, object);
         }

         /*
            Inserts a room into the game.

            Input:
               shared_ptr<entity::Room>

            Output:
               (none)
         */
         inline void insertEntity(string name, std::shared_ptr<entity::Room> room) {

            if (entities.isset(name)) {
               throw entity::EntityException(string("Entity '") + name + "' already exists");
            }

            entities.set(name, room);
            places.set(name, room);
            rooms.set(name, room);
         }

         /*
            Initializes the game, including the event handler, timer and all game
            entities.

            Input:
               Pointer to an instance of Parser (Parser *)
               Game definition filename (string)

            Output:
               True if initialization was successful and false if not (bool)
         */
         bool initialize(Parser *parser, string gamefile);

         /*
            Puts the game into a running state. In a running state, the timer is
            ticking, and player commands are executed.

            To pause and resume a game, simply stop and start it.

            Input: (none)
            Output: (none)
         */
         void start();

         /*
            Puts the game into a stopped state. In a stopped state, the timer is
            not running (though it retains the current time), and player commands
            are not executed.

            To pause and resume a game, simply stop and start it.

            Input: (none)
            Output: (none)
         */
         void stop();

         /*
            Returns the status of the game.

            Input: (none)
            Output: boolean true if the game is running and false if it's not.
         */
         inline bool inProgress() const {return inGame;}

         /*
            Creates a new player and inserts it into the game.  Throws an
            exception if an entity with the given name already exists.

            Input:
               Player name (string)
               Pointer to an output stream (Trogout *)

            Output:
               Player *
         */
         Player *createPlayer(string name, std::unique_ptr<Trogout> outStream,
         std::unique_ptr<Trogin> inStream, std::unique_ptr<Trogout> errStream);

         /*
            Wraps around Timer API.  See timer.h for documentation.
         */
         void insertTimerJob(std::shared_ptr<TimerJob> j);
         void removeTimerJob(std::shared_ptr<TimerJob> j);

         /*
            Gets the current game time (in seconds.)  Note that I can't inline
            this due to forward declaration stuff.  F*#@ me!

            Input: (none)
            Output: time in seconds (unsigned long)
         */
         unsigned long getTime() const;

         /*
            Reads a command from the user and executes it.

            Input: Player executing command
            Output: (none)
         */
         void processCommand(Player *player);

         /*
            Wraps around Vocabulary::insertVerbAction, allowing the client to
            supply its own custom actions. See vocabulary.h for documentation.
         */
         void insertVerbAction(string verb, std::unique_ptr<Action> action);

         /*
            Wraps around Vocabulary::insertSynonym, allowing the client to
            supply its own verb synonyms. See vocabulary.h for documentation.
         */
         inline void insertVerbSynonym(string synonym, string verb) {

            vocabulary.insertVerbSynonym(synonym, verb);
         }

         /*
            Wraps around EventHandler API.  See eventhandler.h for documentation.
         */
         inline void setupEventHandler() {

            events->setup();
         }

         /*
            Wraps around EventHandler API.  See eventhandler.h for documentation.
         */
         inline void addEventListener(event::EventListener *l) {

            events->addListener(l);
         }

         /*
            Wraps around EventHandler API.  See eventhandler.h for documentation.
         */
         inline bool event(const char *event, event::EventArgumentList &args) {

            // make sure global EventListener is always listening
            events->addListener(eventListener.get());
            return events->event(event, args);
         }

         /*
            Wraps around EventHandler API.  See eventhandler.h for documentation.
         */
         inline bool event(const char *event) {

            // make sure global EventListener is always listening
            events->addListener(eventListener.get());
            return events->event(event);
         }
   };
}


#endif

