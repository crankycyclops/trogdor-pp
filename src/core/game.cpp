#include <trogdor/game.h>
#include <trogdor/actions/action.h>
#include <trogdor/timer/timer.h>
#include <trogdor/parser/parser.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/instantiator/instantiators/runtime.h>

#include <trogdor/entities/entity.h>
#include <trogdor/entities/resource.h>
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

#include <trogdor/iostream/placeout.h>
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
      }

      catch (const Exception &e) {
         err() << e.what() << std::endl;
      }
   }

   /***************************************************************************/

   Game::Game(
      std::shared_ptr<serial::Serializable> data,
      std::unique_ptr<Trogerr> gErrStream,
      std::function<std::unique_ptr<Trogout>(Game *)> makeOutStream,
      std::function<std::unique_ptr<Trogerr>(Game *)> makeErrStream
   ) {

         errStream = std::move(gErrStream);
         _deserialize(data, makeOutStream, makeErrStream);
   }

   /***************************************************************************/

   // Empty destructor required here (if I don't include this, the compiler will
   // choke.)
   Game::~Game() {}

   /***************************************************************************/

   void Game::_deserialize(
      std::shared_ptr<serial::Serializable> data,
      std::function<std::unique_ptr<Trogout>(Game *)> makeOutStream,
      std::function<std::unique_ptr<Trogerr>(Game *)> makeErrStream
   ) {

      inGame = std::get<bool>(*data->get("inGame"));

      const std::shared_ptr<serial::Serializable> intro =
         std::get<std::shared_ptr<serial::Serializable>>(*data->get("introduction"));

      introduction.enabled = std::get<bool>(*intro->get("enabled"));
      introduction.text = std::get<std::string>(*intro->get("text"));

      for (const auto &metaVal:
      std::get<std::shared_ptr<serial::Serializable>>(*data->get("meta"))->getAll()) {
         meta[metaVal.first] = std::get<std::string>(metaVal.second);
      }

      defaultPlayer = std::make_unique<entity::Player>(
         this,
         *std::get<std::shared_ptr<serial::Serializable>>(*data->get("defaultPlayer")),
         std::make_unique<NullOut>(),
         std::make_unique<NullErr>()
      );

      const serial::Value entityArr = *data->get("entities");

      for (const auto &entity:
      std::get<std::vector<std::shared_ptr<serial::Serializable>>>(entityArr)) {

         switch (entity::Entity::strToType(
            std::get<std::vector<std::string>>(*entity->get("types")).back()
         )) {

            case entity::ENTITY_RESOURCE:

               insertEntity(
                  std::get<std::string>(*entity->get("name")),
                  std::make_shared<entity::Resource>(this, *entity)
               );

               break;

            case entity::ENTITY_ROOM:

               insertEntity(
                  std::get<std::string>(*entity->get("name")),
                  std::make_shared<entity::Room>(
                     this,
                     *entity,
                     std::make_unique<PlaceOut>(),
                     err().copy()
                  )
               );

               break;

            case entity::ENTITY_OBJECT:

               insertEntity(
                  std::get<std::string>(*entity->get("name")),
                  std::make_shared<entity::Object>(
                     this,
                     *entity,
                     std::make_unique<NullOut>(),
                     err().copy()
                  )
               );

               break;

            case entity::ENTITY_CREATURE:

               insertEntity(
                  std::get<std::string>(*entity->get("name")),
                  std::make_shared<entity::Creature>(
                     this,
                     *entity,
                     std::make_unique<NullOut>(),
                     err().copy()
                  )
               );

               break;

            case entity::ENTITY_PLAYER:

               insertPlayer(
                  std::make_shared<entity::Player>(
                     this,
                     *entity,
                     makeOutStream(this),
                     makeErrStream(this)
                  ),
                  nullptr,
                  true
               );

               break;

            default:
               break;
         }
      }

      L = std::make_unique<LuaState>(
         this,
         *std::get<std::shared_ptr<serial::Serializable>>(*data->get("lua"))
      );

      eventListener = std::make_unique<event::EventListener>(
         *std::get<std::shared_ptr<serial::Serializable>>(*data->get("eventListener")), L
      );

      timer = std::make_unique<Timer>(
         this,
         *std::get<std::shared_ptr<serial::Serializable>>(*data->get("timer"))
      );

      executeCallback("afterDeserialize", nullptr);

      if (inGame) {
         start();
      }
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Game::serialize() {

      bool gameWasStarted = inGame;

      if (gameWasStarted) {
         stop();
      }

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();
      std::shared_ptr<serial::Serializable> serializedIntro = std::make_shared<serial::Serializable>();
      std::shared_ptr<serial::Serializable> serializedMeta = std::make_shared<serial::Serializable>();

      std::vector<std::shared_ptr<serial::Serializable>> serializedEntities;

      for (const auto &entity: entities) {
         serializedEntities.push_back(entity.second->serialize());
      }

      for (const auto &metaItem: meta) {
         serializedMeta->set(metaItem.first, metaItem.second);
      }

      serializedIntro->set("enabled", introduction.enabled);
      serializedIntro->set("text", introduction.text);

      data->set("inGame", gameWasStarted);
      data->set("lua", L->serialize());
      data->set("timer", timer->serialize());
      data->set("introduction", serializedIntro);
      data->set("meta", serializedMeta);
      data->set("defaultPlayer", defaultPlayer->serialize());
      data->set("eventListener", eventListener->serialize());
      data->set("entities", serializedEntities);

      // TODO: if I'm stopping game and timer to ensure consistency while
      // serializing, how do I want to signal to deserialization later that it
      // should be started again? Should that be automatic? Should serialization
      // include extra setting to specify? Will need to think about this.
      if (gameWasStarted) {
         start();
      }

      return data;
   }

   /***************************************************************************/

   void Game::deserialize(
      std::shared_ptr<serial::Serializable> data,
      std::function<std::unique_ptr<Trogout>(Game *)> makeOutStream,
      std::function<std::unique_ptr<Trogerr>(Game *)> makeErrStream
   ) {

      stop();
      meta.clear();
      clearEntities();

      _deserialize(data, makeOutStream, makeErrStream);
   }

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

      // Only return true if we've successfully parsed and instantiated one or
      // more entities
      // TODO: add more rigorous tests, like Room with name "start" exists, and
      // other important sanity checks.
      return entities.size() > 0 ? true : false;
   }

   /***************************************************************************/

   void Game::start() {

      mutex.lock();
      inGame = true;
      timer->start();
      mutex.unlock();

      executeCallback("start", nullptr);
   }

   /***************************************************************************/

   void Game::stop() {

      mutex.lock();
      timer->stop();
      inGame = false;
      mutex.unlock();

      executeCallback("stop", nullptr);
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

         case entity::ENTITY_RESOURCE:
            resources[name] = std::dynamic_pointer_cast<entity::Resource>(entity);
            break;

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

         case entity::ENTITY_RESOURCE:
            resources.erase(name);
            break;

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

   void Game::insertPlayer(
      std::shared_ptr<entity::Player> player,
      std::function<void()> confirmationCallback,
      bool deserialize
   ) {

      executeCallback("beforeInsertPlayer", player);

      // Make sure there are no name conflicts before inserting the new player
      if (entities.find(player->getName()) != entities.end()) {
         throw entity::DuplicateEntity(
            std::string("Entity with name '") + player->getName() + "' already exists"
         );
      }

      // if new player introduction is enabled and we're not deserializing a
      // previously existing game, show it before inserting the new player
      if (!deserialize && introduction.enabled && introduction.text.length() > 0) {

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
      if (!deserialize) {
         player->setLocation(places["start"]);
         places["start"]->insertThing(player);
      }

      mutex.unlock();

      // Player must see an initial description of where they are
      if (!deserialize) {
         player->getLocation().lock()->observe(player, false);
      }

      executeCallback("afterInsertPlayer", player);

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

         executeCallback("removePlayer", players[name]);

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

   void Game::executeCallback(std::string operation, std::any data) {

      std::vector<std::shared_ptr<GameCallback> *> toRemove;

      if (callbacks.end() != callbacks.find(operation)) {
         for (auto &callback: callbacks[operation]) {
            if ((*callback)(data)) {
               toRemove.push_back(&callback);
            }
         }
      }

      // If a callback flagged itself for removal, do so now
      while (toRemove.size()) {
         removeCallback(operation, *toRemove.back());
         toRemove.pop_back();
      }
   }

   /***************************************************************************/

   void Game::addCallback(std::string operation, std::shared_ptr<GameCallback> callback) {

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

   void Game::removeCallback(std::string operation, const std::shared_ptr<GameCallback> &callback) {

      if (callbacks.end() != callbacks.find(operation)) {

         auto it = std::find(callbacks[operation].begin(), callbacks[operation].end(), callback);

         if (it != callbacks[operation].end()) {
            callbacks[operation].erase(it);
         }
      }
   }
}
