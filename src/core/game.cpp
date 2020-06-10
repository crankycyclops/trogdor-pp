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

#include <trogdor/exception/duplicateentity.h>


namespace trogdor {


   Game::Game(std::unique_ptr<Trogerr> e) {

      try {

         errStream = std::move(e);

         inGame = false;
         timer = std::make_unique<Timer>(this);

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
   void Game::initEvents() {

      eventListener->addTrigger("afterGotoLocation", std::make_unique<event::AutoAttackEventTrigger>());
      eventListener->addTrigger("afterDie", std::make_unique<event::DeathDropEventTrigger>());
      eventListener->addTrigger("afterDie", std::make_unique<event::RespawnEventTrigger>());
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
      }

      // In some cases (like the PHP 7 module I'm writing that wraps around
      // this), it's useful to let the exception fall through.
      else {
         parser->parse(gamefile);
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

      if (callbacks.end() != callbacks.find("start")) {
         for (const auto &callback: callbacks["start"]) {
            (*callback)(nullptr);
         }
      }
   }

   /***************************************************************************/

   void Game::stop() {

      resourceMutex.lock();
      timer->stop();
      inGame = false;
      resourceMutex.unlock();

      if (callbacks.end() != callbacks.find("stop")) {
         for (const auto &callback: callbacks["stop"]) {
            (*callback)(nullptr);
         }
      }
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
      if (entities.find(name) != entities.end()) {
         throw entity::DuplicateEntity(
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
      if (entities.find(player->getName()) != entities.end()) {
         throw entity::DuplicateEntity(
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

      resourceMutex.lock();

      entities[player->getName()] = player;
      things[player->getName()] = player;
      beings[player->getName()] = player;
      players[player->getName()] = player;

      player->setGame(this);

      // set Player's initial location
      player->setLocation(places["start"]);
      places["start"]->insertThing(player);

      resourceMutex.unlock();

      // Player must see an initial description of where they are
      player->getLocation().lock()->observe(player, false);

      if (callbacks.end() != callbacks.find("insertPlayer")) {
         for (const auto &callback: callbacks["insertPlayer"]) {
            (*callback)(player);
         }
      }

      // Make sure the player's health information is sent out when they're
      // first inserted into the game. This should be the only time we have to
      // call this method outside of Being::setHealth.
      player->notifyHealth();
   }

   /***************************************************************************/

   void Game::removePlayer(
      const std::string name,
      const std::string message,
      const bool lockOnResourceMutex
   ) {

      if (players.find(name) != players.end()) {

         if (callbacks.end() != callbacks.find("removePlayer")) {
            for (const auto &callback: callbacks["removePlayer"]) {
               (*callback)(players[name]);
            }
         }

         if (message.length()) {
            players[name]->out("system") << message << std::endl;
         }

         // Signal to the player that they're being removed from the game by
         // sending an empty message on the "removed" channel
         players[name]->out("removed") << std::endl;

         // if the Player is located in a Place, make sure to remove it
         if (auto location = players[name]->getLocation().lock()) {
            location->removeThing(players[name]);
         }

         if (lockOnResourceMutex) {
            resourceMutex.lock();
         }

         entities.erase(name);
         things.erase(name);
         beings.erase(name);
         players.erase(name);

         if (lockOnResourceMutex) {
            resourceMutex.unlock();
         }
      }
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

   /***************************************************************************/

   void Game::addCallback(std::string operation, std::shared_ptr<std::function<void(std::any)>> callback) {

      callbacks[operation].push_back(callback);
   }

   /***************************************************************************/

   size_t Game::removeCallbacks(std::string operation) {

      if (callbacks.end() != callbacks.find(operation)) {

         size_t size = callbacks[operation].size();

         callbacks.erase(operation);
         return size;
      }

      return 0;
   }

   /***************************************************************************/

   void Game::removeCallback(std::string operation, const std::shared_ptr<std::function<void(std::any)>> &callback) {

      if (callbacks.end() != callbacks.find(operation)) {

         auto it = std::find(callbacks[operation].begin(), callbacks[operation].end(), callback);

         if (it != callbacks[operation].end()) {
            callbacks[operation].erase(it);
         }
      }
   }
}
