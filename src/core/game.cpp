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

#include <trogdor/iostream/nullin.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>
#include <trogdor/iostream/trogout.h>
#include <trogdor/iostream/trogerr.h>

#include <trogdor/exception/entityexception.h>


namespace trogdor {


   Game::Game(std::unique_ptr<Trogerr> e) {

      try {

         errStream = std::move(e);

         inGame = false;
         timer = std::make_unique<Timer>(this);
         events = std::make_unique<event::EventHandler>();

         introduction.enabled           = DEFAULT_INTRODUCTION_ENABLED;
         introduction.text              = "";

         defaultPlayer = std::make_unique<entity::Player>(
            this,
            "default",
            std::make_unique<NullOut>(),
            std::make_unique<NullIn>(),
            std::make_unique<NullErr>()
         );

         L = std::make_shared<LuaState>(this);
         eventListener = std::make_unique<event::EventListener>();

         lastCommand = nullptr;
      }

      catch (const Exception &e) {
         err() << e.what() << std::endl;
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

   bool Game::initialize(Parser *parser, std::string gamefile, bool handleExceptions) {

      if (handleExceptions) {

         try {
            parser->parse(gamefile);
         }

         catch (const Exception &e) {
            err() << e.what() << std::endl;
            return false;
         }

         initEvents();
      }

      // In some cases (like the PHP 7 module I'm writing that wraps around
      // this), it's useful to let the exception fall through.
      else {
         parser->parse(gamefile);
      }

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

   void Game::deactivate() {

      resourceMutex.lock();
      inGame = false;
      timer->deactivate();
      resourceMutex.unlock();
   }

   /***************************************************************************/

   void Game::shutdown() {

      timer->shutdown();
   }

   /***************************************************************************/

   std::shared_ptr<Player> Game::createPlayer(std::string name, std::unique_ptr<Trogout> outStream,
   std::unique_ptr<Trogin> inStream, std::unique_ptr<Trogerr> errStream) {

      // Make sure there are no name conflicts before creating the new player
      if (entities.isEntitySet(name)) {
         throw entity::EntityException(
            std::string("Entity with name '") + name + "' already exists"
         );
      }

      // clone the default player, giving it the specified name
      std::shared_ptr<Player> player = std::make_shared<Player>(
         *defaultPlayer, name, std::move(outStream), std::move(inStream), std::move(errStream)
      );

      return player;
   }

   /***************************************************************************/

   void Game::insertPlayer(std::shared_ptr<Player> player,
   std::function<void()> confirmationCallback) {

      // Make sure there are no name conflicts before inserting the new player
      if (entities.isEntitySet(player->getName())) {
         throw entity::EntityException(
            std::string("Entity with name '") + player->getName() + "' already exists"
         );
      }

      // if new player introduction is enabled, show it before inserting
      // the new player into the game
      if (introduction.enabled && introduction.text.length() > 0) {

         player->out() << introduction.text << std::endl;

         if (confirmationCallback) {
            confirmationCallback();
         }
      }

      entities.set(player->getName(), player);
      things.set(player->getName(), player);
      beings.set(player->getName(), player);
      players.set(player->getName(), player);

      // set Player's initial location
      player->setLocation(places.get("start"));
      places.get("start")->insertThing(player);

      // Player must see an initial description of where they are
      player->getLocation()->observe(player, false);
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

      // don't do anything if the user didn't actually enter a command
      if (command->isNull()) {
         return;
      }

      // do nothing if we're not in a running state
      else if (!inGame) {
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
