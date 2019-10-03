#include "include/game.h"
#include "include/action.h"
#include "include/timer/timer.h"
#include "include/parser/parser.h"
#include "include/instantiator/instantiators/runtime.h"

#include "include/entities/entity.h"
#include "include/entities/place.h"
#include "include/entities/room.h"
#include "include/entities/thing.h"
#include "include/entities/being.h"
#include "include/entities/player.h"
#include "include/entities/creature.h"
#include "include/entities/object.h"

#include "include/event/triggers/autoattack.h"
#include "include/event/triggers/deathdrop.h"
#include "include/event/triggers/respawn.h"

#include "include/iostream/nullout.h"
#include "include/iostream/nullin.h"
#include "include/iostream/trogout.h"

#include "include/exception/entityexception.h"


using namespace std;

namespace trogdor {


   Game::Game(std::unique_ptr<Trogout> e) {

      try {

         errStream = std::move(e);

         inGame = false;
         timer = make_unique<Timer>(this);
         events = make_unique<event::EventHandler>();

         introduction.enabled           = DEFAULT_INTRODUCTION_ENABLED;
         introduction.pauseWhileReading = DEFAULT_INTRODUCTION_PAUSE;
         introduction.text              = "";

         defaultPlayer = make_unique<entity::Player>(
            this,
            "default",
            make_unique<NullOut>(),
            make_unique<NullIn>(),
            make_unique<NullOut>()
         );

         L = std::make_shared<LuaState>(this);
         eventListener = std::make_unique<event::EventListener>();

         lastCommand = nullptr;
      }

      catch (const Exception &e) {
         *errStream << e.what() << endl;
      }
   }

   /***************************************************************************/

   // Empty destructor required here (if I don't include this, the compiler will
   // choke.)
   Game::~Game() {}

   /***************************************************************************/

   // NOTE: order is important!
   // TODO: does it make more sense for these events to be added via the trigger
   // objects themselves?
   void Game::initEvents() {

      eventListener->add("afterGotoLocation", new AutoAttackEventTrigger());
      eventListener->add("afterDie", new DeathDropEventTrigger());
      eventListener->add("afterDie", new RespawnEventTrigger());
   }

   /***************************************************************************/

   bool Game::initialize(Parser *parser, string gamefile) {

      try {
         parser->parse(gamefile);
      }

      catch (const Exception &e) {
         *errStream << e.what() << endl;
         return false;
      }

      initEvents();

      return true;
   }

   /***************************************************************************/

   void Game::start() {

      resourceMutex.lock();
      inGame = true;
      timer->start();
      resourceMutex.unlock();
   }

   /***************************************************************************/

   void Game::stop() {

      resourceMutex.lock();
      timer->stop();
      inGame = false;
      resourceMutex.unlock();
   }

   /***************************************************************************/

   Player *Game::createPlayer(string name, std::unique_ptr<Trogout> outStream,
   std::unique_ptr<Trogin> inStream, std::unique_ptr<Trogout> errStream) {

      if (entities.isset(name)) {
         throw entity::EntityException(
            string("Entity with name '") + name + "' already exists"
         );
      }

      // clone the default player, giving it the specified name
      std::shared_ptr<Player> player = make_shared<Player>(
         *defaultPlayer, name, std::move(outStream), std::move(inStream), std::move(errStream)
      );

      // if new player introduction is enabled, show it before inserting
      // the new player into the game
      // TODO: this works fine for a single player game, but in a
      // multi-player environment, the game can't stop every time a player
      // has to read the introduction...
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

      entities.set(name, player);
      things.set(name, player);
      beings.set(name, player);
      players.set(name, player);

      // set Player's initial location
      player->setLocation(places.get("start"));
      places.get("start")->insertThing(player);

      // Player must see an initial description of where they are
      player->getLocation()->observe(player, false);

      return player.get();
   }

   /***************************************************************************/

   void Game::insertTimerJob(std::shared_ptr<TimerJob> j) {

      timer->insertJob(j);
   }

   /***************************************************************************/

   void Game::removeTimerJob(std::shared_ptr<TimerJob> j) {

      timer->removeJob(j);
   }

   /***************************************************************************/

   unsigned long Game::getTime() const {

      return timer->getTime();
   }

   /***************************************************************************/

   void Game::processCommand(Player *player) {

      std::shared_ptr<Command> command = make_shared<Command>(vocabulary);
      command->read(player);

      // do nothing if we're not in a running state
      if (!inGame) {
         player->out() << "Game is stopped and not accepting commands." << endl;
         return;
      }

      if (!command->isInvalid()) {

         // player is attempting to re-execute his last command
         if (0 == command->getVerb().compare("a") ||
         0 == command->getVerb().compare("again")) {

            if (nullptr != lastCommand) {
               command = lastCommand;
            }

            else {
               player->out() << "You haven't entered any commands yet!" << endl;
               return;
            }
         }

         string verb = command->getVerb();
         Action *action = vocabulary.getVerbAction(verb);

         if (0 == action || !action->checkSyntax(command)) {
            player->out() << "Sorry, I don't understand you." << endl;
         }

         else {
            resourceMutex.lock();
            action->execute(player, command, this);
            resourceMutex.unlock();
         }
      }

      else {
         player->out() << "Sorry, I don't understand you." << endl;
      }

      lastCommand = command;
   }

   /***************************************************************************/

   void Game::insertVerbAction(string verb, std::unique_ptr<Action> action) {

      vocabulary.insertVerbAction(verb, std::move(action));
   }
}

