#ifndef GAME_H
#define GAME_H


#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <pthread.h>

#include "command.h"
#include "eventhandler.h"
#include "entitymap.h"


using namespace std;

namespace core {


   class ActionMap; // resolves circular dependency ActionMap <-> Game
   class Timer;     // resolves circular dependency Timer <-> Game
   class Parser;    // resolves circular dependency Parser <-> Game


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

      private:

         bool       inGame;        // whether or not a game is in progress
         Parser     *parser;       // parses game.xml and constructs entities
         ActionMap  *actions;      // maps verbs to actions
         Command    *lastCommand;  // the last executed command
         Timer      *timer;

         // TODO: going to need an event listener here too
         event::EventHandler *events;

         // Hash table of all entities in the game
         entity::EntityMap    entities;
         entity::PlaceMap     places;
         entity::ThingMap     things;
         entity::RoomMap      rooms;
         entity::BeingMap     beings;
         entity::PlayerMap    players;
         entity::CreatureMap  creatures;
         entity::ItemMap      items;
         entity::ObjectMap    objects;

         /*
            Called by initialize().  This initializes game actions and maps
            them to verbs.

            Input: (none)
            Output: (none)
         */
         void initActions();

      public:

         /* lock on this to keep data consistent between threads */
         pthread_mutex_t resourceMutex;

         /* lock on this to synchronize timer actions */
         pthread_mutex_t timerMutex;

         ostream *trogerr;  /* error output stream */
         ostream *trogout;  /* console output stream */
         istream *trogin;   /* input stream */

         /*
            Constructor for the Game class.
         */
         Game();

         /*
            Destructor for the Game class.
         */
         ~Game();

         /*
            Sets the game's error output stream.

            Input: new output stream (ostream)
            Output: (none)
         */
         inline void setTrogerr(ostream *newerr) {trogerr = newerr;}

         /*
            Sets the game's standard output stream.

            Input: new output stream (ostream)
            Output: (none)
         */
         inline void setTrogout(ostream *newout) {trogout = newout;}

         /*
            Sets the game's input stream.

            Input: new input stream (istream)
            Output: (none)
         */
         inline void setTrogin(istream *newin) {trogin = newin;}

         /*
            Creates a new player and inserts it into the game.  Throws an
            exception if an entity with the given name already exists.

            Input:
               Player name (string)

            Output:
               Pointer to Player object
         */
         inline Player *createPlayer(string name) {

            if (entities.isset(name)) {
               stringstream s;
               s << "Entity with name '" << name << "' already exists";
               throw s.str();
            }

            // TODO: implement Player::copy constructor and use that with default
            // (and then set the name after)
            // OR: maybe have a copy method that copies just properties and
            // stuff after construction?
            Player *player = new Player(this, name);

            entities.set(name, player);
            things.set(name, player);
            beings.set(name, player);
            players.set(name, player);

            // set Player's initial location
            player->setLocation(places.get("start"));

            return player;
         }

         /*
            Removes a player from the game.  Throws an exception if the player
            given by the specified name doesn't exist.

            Input:
               Name of player (string)

            Output:
               (none)
         */
         inline void removePlayer(const string name) {

            if (!players.isset(name)) {
               stringstream s;
               s << "Player with name '" << name << "' doesn't exist";
               throw s.str();
            }

            Player *player = players.get(name);

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
            Initializes the game, including the event handler, timer and all game
            entities.

            Input: Game definition XML file (default is game.xml)
            Output: True if initialization was successful and false if not.
         */
         bool initialize(string gameXML = "game.xml");

         /*
            Puts the game into a running state.

            Input: (none)
            Output: (none)
         */
         void start();

         /*
            Puts the game into a stopped state.

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
   };
}


#endif

