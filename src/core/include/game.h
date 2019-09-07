#ifndef GAME_H
#define GAME_H


#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <thread>
#include <mutex>

#include "thread.h"
#include "command.h"
#include "event/eventhandler.h"
#include "entitymap.h"
#include "iostream/trogout.h"


using namespace std;

namespace trogdor { namespace core {


   class ActionMap; // resolves circular dependency ActionMap <-> Game
   class Timer;     // resolves circular dependency Timer <-> Game
   class Parser;    // resolves circular dependency Parser <-> Game

   class TimerJob;


   /*
      Each Game object represents a self contained game and contains all
      game-related data.  A standard lifecycle of a game object -- assuming it
      exists in main() -- might look something like:

      Game currentGame = new Game();

      currentGame.initialize();

      currentGame.start();
      while (currentGame.inProgress()) {
         ...
         read/execute a command;
         ...
      }

      delete currentGame;
      return EXIT_SUCCESS;
   */
   class Game {

      public:

         typedef unordered_map<string, string> StringMap;

         static const bool DEFAULT_INTRODUCTION_ENABLED = false;
         static const bool DEFAULT_INTRODUCTION_PAUSE   = false;

      private:

         bool       inGame;        // whether or not a game is in progress
         Parser     *parser;       // parses game.xml and constructs entities
         ActionMap  *actions;      // maps verbs to actions
         Command    *lastCommand;  // the last executed command
         Timer      *timer;

         StringMap    meta;          // meta data
         StringMap    synonyms;      // verb synonyms

         // used to call subscribed event listeners
         event::EventHandler *events;

         // Global Lua state for the entire game
         LuaState *L;

         // Global EventListener for the entire game
         event::EventListener *eventListener;

         // default player configuration
         entity::Player *defaultPlayer;

         // Hash table of all entities in the game
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
         Trogout *errStream;

         /*
            Initializes Entities from what was parsed.

            Input:
               (none)

            Output:
               (none)
         */
         void initEntities();

         /*
            Called by initialize().  This initializes game actions and maps
            them to verbs.

            Input: (none)
            Output: (none)
         */
         void initActions();

         /*
            Called by initialize().  This initializes event handling in the game.

            Input: (none)
            Output: (none)
         */
         void initEvents();

      public:

         /* lock on this to keep data consistent between threads */
         mutex_t resourceMutex;

         /* lock on this to synchronize timer actions */
         std::mutex timerMutex;

         /*
            Constructor for the Game class.
         */
         Game(Trogout *e);

         /*
            Destructor for the Game class.
         */
         ~Game();

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
            Gets a synonym.  If the value isn't set, an empty string is returned.

            Input:
               synonym (string)

            Output:
               associated verb (string)
         */
         inline string getSynonym(string synonym) const {

            if (synonyms.find(synonym) == synonyms.end()) {
               return "";
            }

            return synonyms.find(synonym)->second;
         }

         /*
            Sets a synonym => verb association.

            Input:
               synonym (string)
               verb (string)

            Output:
               (none)
         */
         inline void setSynonym(string synonym, string verb) {synonyms[synonym] = verb;}

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
            Creates a new player and inserts it into the game.  Throws an
            exception if an entity with the given name already exists.

            Input:
               Player name (string)
               Pointer to an output stream (Trogout *)

            Output:
               Pointer to Player object
         */
         inline Player *createPlayer(string name, Trogout *outStream,
         Trogin *inStream, Trogout *errStream) {

            if (entities.isset(name)) {
               stringstream s;
               s << "Entity with name '" << name << "' already exists";
               throw s.str();
            }

            // clone the default player, giving it the specified name
            Player *player = new Player(*defaultPlayer, name, outStream,
               inStream, errStream);

            // if new player introduction is enabled, show it before inserting
            // the new player into the game
            if (introduction.enabled && introduction.text.length() > 0) {

               if (introduction.pauseWhileReading) {
                  stop();
               }

               // we really only need this to give player->in() something to do
               string blah;

               player->out() << introduction.text << endl << endl;
               player->out() << "Press enter to start." << endl;
               player->in() >> blah;
               player->out() << endl;

               if (introduction.pauseWhileReading) {
                  start();
               }
            }

            // TODO: set other attributes from default

            entities.set(name, player);
            things.set(name, player);
            beings.set(name, player);
            players.set(name, player);

            // set Player's initial location
            player->setLocation(places.get("start"));
            places.get("start")->insertThing(player);

            // Player must see an initial description of where they are
            player->getLocation()->observe(player, false);

            return player;
         }

         /*
            Removes a player from the game.  Throws an exception if the player
            given by the specified name doesn't exist.

            Input:
               Name of player (string)
               Message to output to player before removing (string: default is none)

            Output:
               (none)
         */
         inline void removePlayer(const string name, const string message = "") {

            if (!players.isset(name)) {
               stringstream s;
               s << "Player with name '" << name << "' doesn't exist";
               throw s.str();
            }

            Player *player = players.get(name);

            if (message.length()) {
               player->out("notifications") << message << endl;
            }

            entities.erase(name);
            things.erase(name);
            beings.erase(name);
            players.erase(name);

            delete player;
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
            Returns the Player object associated with the specified player name.
            Throws an exception if the specified player name doesn't exist.

            Input:
               Name of player (string)

            Output:
               Player *
         */
         inline Player *getPlayer(const string name) {

            if (!players.isset(name)) {
               stringstream s;
               s << "Player with name '" << name << "' doesn't exist";
               throw s.str();
            }

            return players.get(name);
         }

         /*
            Initializes the game, including the event handler, timer and all game
            entities.

            Input: Game definition XML file (default is game.xml)
            Output: True if initialization was successful and false if not.
         */
         bool initialize(string gameXML = "game.xml");

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
            Wraps around Timer API.  See timer.h for documentation.
         */
         void insertTimerJob(TimerJob *j);
         void removeTimerJob(TimerJob *j);

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
            events->addListener(eventListener);
            return events->event(event, args);
         }

         /*
            Wraps around EventHandler API.  See eventhandler.h for documentation.
         */
         inline bool event(const char *event) {

            // make sure global EventListener is always listening
            events->addListener(eventListener);
            return events->event(event);
         }
   };
}}


#endif

