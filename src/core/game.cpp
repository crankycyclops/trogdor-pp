#include <trogdor/game.h>
#include <trogdor/actions/action.h>
#include <trogdor/timer/timer.h>
#include <trogdor/parser/parser.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/instantiator/instantiators/runtime.h>

#include <trogdor/entities/entity.h>
#include <trogdor/entities/tangible.h>
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

   std::unique_ptr<Runtime> Game::makeInstantiator() {

      return std::make_unique<Runtime>(getVocabulary(), this);
   }

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
      return true;
   }

   /***************************************************************************/

   void Game::start() {

      mutex.lock();
      inGame = true;
      timer->start();
      mutex.unlock();

      if (callbacks.end() != callbacks.find("start")) {
         for (const auto &callback: callbacks["start"]) {
            (*callback)(nullptr);
         }
      }
   }

   /***************************************************************************/

   void Game::stop() {

      mutex.lock();
      timer->stop();
      inGame = false;
      mutex.unlock();

      if (callbacks.end() != callbacks.find("stop")) {
         for (const auto &callback: callbacks["stop"]) {
            (*callback)(nullptr);
         }
      }
   }

   /***************************************************************************/

   void Game::deactivate() {

      mutex.lock();
      inGame = false;
      timer->deactivate();
      mutex.unlock();
   }

   /***************************************************************************/

   void Game::shutdown() {

      timer->shutdown();
   }

   /***************************************************************************/

   std::shared_ptr<entity::Player> Game::createPlayer(std::string name,
   std::unique_ptr<Trogout> outStream, std::unique_ptr<Trogerr> errStream) {

      // Make sure there are no name conflicts before creating the new player
      if (entities.find(name) != entities.end()) {
         throw entity::DuplicateEntity(
            std::string("Entity with name '") + name + "' already exists"
         );
      }

      // clone the default player, giving it the specified name
      std::shared_ptr<entity::Player> player = std::make_shared<entity::Player>(
         *defaultPlayer, name, std::move(outStream), std::move(errStream)
      );

      return player;
   }

   /***************************************************************************/

   void Game::insertEntity(std::string name, std::shared_ptr<entity::Entity> entity) {

      if (entities.find(name) != entities.end()) {
         throw entity::EntityException(std::string("Entity '") + name + "' already exists");
      }

      mutex.lock();

      switch (entity->getType()) {

         case entity::ENTITY_ROOM:
            tangibles[name] = std::dynamic_pointer_cast<entity::Tangible>(entity);
            places[name] = std::dynamic_pointer_cast<entity::Place>(entity);
            rooms[name] = std::dynamic_pointer_cast<entity::Room>(entity);
            break;

         case entity::ENTITY_OBJECT:
            tangibles[name] = std::dynamic_pointer_cast<entity::Tangible>(entity);
            things[name] = std::dynamic_pointer_cast<entity::Thing>(entity);
            objects[name] = std::dynamic_pointer_cast<entity::Object>(entity);
            break;

         case entity::ENTITY_CREATURE:
            tangibles[name] = std::dynamic_pointer_cast<entity::Tangible>(entity);
            things[name] = std::dynamic_pointer_cast<entity::Thing>(entity);
            beings[name] = std::dynamic_pointer_cast<entity::Being>(entity);
            creatures[name] = std::dynamic_pointer_cast<entity::Creature>(entity);
            break;

         case entity::ENTITY_PLAYER:
            throw UndefinedException("Game::insertEntity: Use Game::insertPlayer instead");

         default:
            throw UndefinedException("Game::insertEntity: unsupported entity type");
      }

      entities[name] = entity;
      entity->setGame(this);

      mutex.unlock();
   }

   /***************************************************************************/

   bool Game::removeEntity(std::string name) {

      if (entities.end() == entities.find(name)) {
         throw entity::EntityException("Entity not found");
      }

      // Removing the room named "start" would lead to undefined behavior.
      else if (0 == name.compare("start")) {
         throw UndefinedException("Cannot remove \"start\" as this would lead to undefined behavior.");
      }

      // If the Entity is contained in a location, it will have to be removed
      // first.
      else if (
         entities[name]->isType(entity::EntityType::ENTITY_THING) &&
         std::static_pointer_cast<entity::Thing>(entities[name])->getLocation().lock()
      ) {
         return false;
      }

      // If the Entity is contained in a Being's inventory, it will have to be
      // removed first.
      else if (
         entities[name]->isType(entity::EntityType::ENTITY_OBJECT) &&
         std::static_pointer_cast<entity::Object>(entities[name])->getOwner().lock()
      ) {
         return false;
      }

      mutex.lock();

      switch (entities[name]->getType()) {

         case entity::ENTITY_ROOM:
            tangibles.erase(name);
            places.erase(name);
            rooms.erase(name);
            break;

         case entity::ENTITY_OBJECT:
            tangibles.erase(name);
            things.erase(name);
            objects.erase(name);
            break;

         case entity::ENTITY_CREATURE:
            tangibles.erase(name);
            things.erase(name);
            beings.erase(name);
            creatures.erase(name);
            break;

         case entity::ENTITY_PLAYER:
            removePlayer(name, "");
            mutex.unlock();
            return true;

         default:
            throw UndefinedException("Game::removeEntity: unsupported entity type");
      }

      entities[name]->setGame(nullptr);
      entities.erase(name);

      mutex.unlock();
      return true;
   }

   /***************************************************************************/

   void Game::insertPlayer(std::shared_ptr<entity::Player> player,
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

      mutex.lock();

      entities[player->getName()] = player;
      tangibles[player->getName()] = player;
      things[player->getName()] = player;
      beings[player->getName()] = player;
      players[player->getName()] = player;

      player->setGame(this);

      // set Player's initial location
      player->setLocation(places["start"]);
      places["start"]->insertThing(player);

      mutex.unlock();

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
      const std::string message
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

         mutex.lock();

         entities[name]->setGame(nullptr);

         entities.erase(name);
         tangibles.erase(name);
         things.erase(name);
         beings.erase(name);
         players.erase(name);

         mutex.unlock();
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

   bool Game::executeAction(entity::Player *player, const Command &command) {

      Action *action = vocabulary.getVerbAction(command.getVerb());

      if (nullptr == action || !action->checkSyntax(command)) {
         return false;
      }

      mutex.lock();
      action->execute(player, command, this);
      mutex.unlock();

      return true;
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
