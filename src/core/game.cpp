#include <trogdor/game.h>
#include <trogdor/action.h>
#include <trogdor/timer/timer.h>
#include <trogdor/parser/parser.h>
#include <trogdor/instantiator/instantiators/runtime.h>

#include <trogdor/entities/entity.h>
#include <trogdor/entities/place.h>
#include <trogdor/entities/room.h>
#include <trogdor/entities/thing.h>
#include <trogdor/entities/being.h>
#include <trogdor/entities/player.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/event/triggers/autoattack.h>
#include <trogdor/event/triggers/deathdrop.h>
#include <trogdor/event/triggers/respawn.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullin.h>
#include <trogdor/iostream/trogout.h>

#include <trogdor/exception/entityexception.h>


namespace trogdor {


   Game::Game(std::unique_ptr<Trogout> e) {

      try {

         errStream = std::move(e);

         inGame = false;
         timer = std::make_unique<Timer>(this);
         events = std::make_unique<event::EventHandler>();

         introduction.enabled           = DEFAULT_INTRODUCTION_ENABLED;
         introduction.pauseWhileReading = DEFAULT_INTRODUCTION_PAUSE;
         introduction.text              = "";

         defaultPlayer = std::make_unique<entity::Player>(
            this,
            "default",
            std::make_unique<NullOut>(),
            std::make_unique<NullIn>(),
            std::make_unique<NullOut>()
         );

         L = std::make_shared<LuaState>(this);
         eventListener = std::make_unique<event::EventListener>();

         lastCommand = nullptr;
      }

      catch (const Exception &e) {
         *errStream << e.what() << std::endl;
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

   bool Game::initialize(Parser *parser, std::string gamefile) {

      try {
         parser->parse(gamefile);
      }

      catch (const Exception &e) {
         *errStream << e.what() << std::endl;
         return false;
      }

      initEvents();

      // Only return true if we've successfully parsed and instantiated one or
      // more entities
      // TODO: add more rigorous tests, like Room with name "start" exists, and
      // other important sanity checks.
      return entities.size() > 0 ? true : false;
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

   Player *Game::createPlayer(std::string name, std::unique_ptr<Trogout> outStream,
   std::unique_ptr<Trogin> inStream, std::unique_ptr<Trogout> errStream) {

      if (entities.isEntitySet(name)) {
         throw entity::EntityException(
            std::string("Entity with name '") + name + "' already exists"
         );
      }

      // clone the default player, giving it the specified name
      std::shared_ptr<Player> player = std::make_shared<Player>(
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
         std::string blah;

         player->out() << introduction.text << std::endl << std::endl;
         player->out() << "Press enter to start." << std::endl;
         player->in() >> blah;
         player->out() << std::endl;

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

      std::shared_ptr<Command> command = std::make_shared<Command>(vocabulary);
      command->read(player);

      // do nothing if we're not in a running state
      if (!inGame) {
         player->out() << "Game is stopped and not accepting commands." << std::endl;
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
               player->out() << "You haven't entered any commands yet!" << std::endl;
               return;
            }
         }

         std::string verb = command->getVerb();
         Action *action = vocabulary.getVerbAction(verb);

         if (nullptr == action || !action->checkSyntax(command)) {
            player->out() << "Sorry, I don't understand you." << std::endl;
         }

         else {
            resourceMutex.lock();
            action->execute(player, command, this);
            resourceMutex.unlock();
         }
      }

      else {
         player->out() << "Sorry, I don't understand you." << std::endl;
      }

      lastCommand = command;
   }

   /***************************************************************************/

   void Game::insertVerbAction(std::string verb, std::unique_ptr<Action> action) {

      vocabulary.insertVerbAction(verb, std::move(action));
   }
}
