#include <string>
#include <memory>
#include <algorithm>

#include <trogdor/game.h>
#include <trogdor/utility.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/placeout.h>

#include <trogdor/timer/jobs/wander.h>
#include <trogdor/event/triggers/luaeventtrigger.h>

#include <trogdor/exception/entityexception.h>
#include <trogdor/exception/undefinedexception.h>
#include <trogdor/exception/validationexception.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/player.h>

#include <trogdor/instantiator/instantiators/runtime.h>


namespace trogdor {

   Runtime::Runtime(const Vocabulary &v, Game *g): Instantiator(v) {

      game = g;

      registerOperations();
      mapGameSetters();
      mapEntitySetters();
   }

   /***************************************************************************/

   void Runtime::afterInstantiate() {

      // For each creature, check if wandering was enabled, and if so, insert a
      // timer job for it
      for (auto &creature: game->getCreatures()) {

         if (creature.second->getWanderEnabled()) {
            game->insertTimerJob(std::make_shared<WanderTimerJob>(game, creature.second->getWanderInterval(),
               -1, creature.second->getWanderInterval(), creature.second.get()));
         }
      }
   }

   /***************************************************************************/

   // Private method
   void Runtime::registerOperations() {

      registerOperation(DEFINE_DIRECTION, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         game->insertDirection(operation->getChildren()[0]->getValue());
      });

      /**********/

      registerOperation(DEFINE_DIRECTION_SYNONYM, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string originalDirection = operation->getChildren()[0]->getValue();
         std::string synonym = operation->getChildren()[1]->getValue();

         game->insertDirectionSynonym(synonym, originalDirection);
      });

      /**********/

      registerOperation(DEFINE_VERB_SYNONYM, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string verb = operation->getChildren()[0]->getValue();
         std::string synonym = operation->getChildren()[1]->getValue();

         game->insertVerbSynonym(synonym, verb);
      });

      /**********/

      registerOperation(DEFINE_ENTITY, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::shared_ptr<Entity> entity;

         std::string entityName = operation->getChildren()[0]->getValue();
         std::string className = operation->getChildren()[2]->getValue();
         entity::EntityType entityType = entity::Entity::strToType(
            operation->getChildren()[1]->getValue()
         );

         // Resources optionally take a fourth parameter which is a custom
         // plural name.
         std::optional<std::string> plural = std::nullopt;

         if (4 == operation->getChildren().size()) {
            plural = operation->getChildren()[3]->getValue();
         }

         switch (entityType) {

            case entity::ENTITY_RESOURCE:

               // Entity has no class, so create a blank Entity
               if (
                  0 == className.compare("") ||
                  0 == className.compare(Entity::typeToStr(entity::ENTITY_RESOURCE))
               ) {
                  entity = std::make_shared<entity::Resource>(
                     game,
                     entityName,
                     std::nullopt,
                     std::nullopt,
                     false,
                     plural
                  );
               }

               // Entity has a class, so copy the class's prototype
               else {
                  entity = std::make_shared<entity::Resource>(
                     *(dynamic_cast<entity::Resource *>(typeClasses[className].get())),
                     entityName,
                     plural
                  );
               }

               break;

            case entity::ENTITY_ROOM:

               // Entity has no class, so create a blank Entity
               if (
                  0 == className.compare("") ||
                  0 == className.compare(Entity::typeToStr(entity::ENTITY_ROOM))
               ) {
                  entity = std::make_shared<entity::Room>(
                     game, entityName, std::make_unique<PlaceOut>(), game->err().copy()
                 );
               }

               // Entity has a class, so copy the class's prototype
               else {
                  entity = std::make_shared<entity::Room>(
                     *(dynamic_cast<entity::Room *>(typeClasses[className].get())),
                     entityName
                  );
               }

               break;

            case entity::ENTITY_OBJECT:

               if (
                  0 == className.compare("") ||
                  0 == className.compare(Entity::typeToStr(entity::ENTITY_OBJECT))
               ) {
                  entity = std::make_shared<entity::Object>(
                     game, entityName, std::make_unique<NullOut>(), game->err().copy()
                  );
               }

               else {
                  entity = std::make_shared<entity::Object>(
                     *(dynamic_cast<entity::Object *>(typeClasses[className].get())),
                     entityName
                  );
               }

               break;

            case entity::ENTITY_CREATURE:

               if (
                  0 == className.compare("") ||
                  0 == className.compare(Entity::typeToStr(entity::ENTITY_CREATURE))
               ) {
                  // TODO: should Creatures have some kind of special input stream?
                  entity = std::make_shared<entity::Creature>(
                     game, entityName, std::make_unique<NullOut>(), game->err().copy()
                  );
               }

               else {
                  entity = std::make_shared<entity::Creature>(
                     *(dynamic_cast<entity::Creature *>(typeClasses[className].get())),
                     entityName
                  );
               }

               break;

            case entity::ENTITY_PLAYER:
               throw UndefinedException("TODO: haven't had a reason to instantiate Player objects yet.");

            default:
               throw UndefinedException("DEFINE_ENTITY: unsupported entity type. This is a bug.");
         }

         entity->setClass(0 == className.compare("") ? entity->getTypeName() : className);
         game->insertEntity(entityName, entity);
      });

      /**********/

      registerOperation(DEFINE_ENTITY_CLASS, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         // Entity classes are represented as model Entity objects that can be copied
         std::unique_ptr<Entity> entity;

         std::string className = operation->getChildren()[0]->getValue();
         entity::EntityType classType = entity::Entity::strToType(
            operation->getChildren()[1]->getValue()
         );

         switch (classType) {

            case entity::ENTITY_RESOURCE:
               entity = std::make_unique<Resource>(game, className);
               break;

            case entity::ENTITY_ROOM:
               entity = std::make_unique<Room>(
                  game, className, std::make_unique<PlaceOut>(), game->err().copy()
               );
               break;

            case entity::ENTITY_OBJECT:
               entity = std::make_unique<Object>(
                  game, className, std::make_unique<NullOut>(), game->err().copy()
               );
               break;

            case entity::ENTITY_CREATURE:
               // TODO: should Creatures have some kind of special input stream?
               entity = std::make_unique<Creature>(
                  game, className, std::make_unique<NullOut>(), game->err().copy()
               );
               break;

            case entity::ENTITY_PLAYER:
               throw UndefinedException("Class cannot be defined for type Player (yet)");

            default:
               throw UndefinedException("DEFINE_ENTITY_CLASS: class defined for unsupported entity type. This is a bug.");
         }

         // for type checking
         entity->setClass(className);
         entity->setProperty("title", className);

         typeClasses[className] = std::move(entity);
      });

      /**********/

      registerOperation(SET_MESSAGE, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string messageName = operation->getChildren()[1]->getValue();
         std::string message = operation->getChildren()[2]->getValue();

         if (0 == targetType.compare("entity")) {
            game->getEntity(
               operation->getChildren()[3]->getValue()
            )->setMessage(messageName, message);
         }

         else if (0 == targetType.compare("class")) {
            typeClasses[operation->getChildren()[3]->getValue()]->setMessage(messageName, message);
         }

         else {
            game->getDefaultPlayer()->setMessage(messageName, message);
         }
      });

      /**********/

      registerOperation(SET_TAG, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string tag = operation->getChildren()[1]->getValue();

         if (0 == targetType.compare("entity")) {
            game->getEntity(
               operation->getChildren()[2]->getValue()
            )->setTag(tag);
         }

         else if (0 == targetType.compare("class")) {
            typeClasses[operation->getChildren()[2]->getValue()]->setTag(tag);
         }

         else {
            game->getDefaultPlayer()->setTag(tag);
         }
      });

      /**********/

      registerOperation(REMOVE_TAG, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string tag = operation->getChildren()[1]->getValue();

         if (0 == targetType.compare("entity")) {
            game->getEntity(
               operation->getChildren()[2]->getValue()
            )->removeTag(tag);
         }

         else if (0 == targetType.compare("class")) {
            typeClasses[operation->getChildren()[2]->getValue()]->removeTag(tag);
         }

         else {
            game->getDefaultPlayer()->removeTag(tag);
         }
      });

      /**********/

      registerOperation(LOAD_SCRIPT, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string scriptMode = operation->getChildren()[1]->getValue();
         std::string script = operation->getChildren()[2]->getValue();

         if (0 == targetType.compare("entity")) {

            std::string entityName = operation->getChildren()[3]->getValue();

            if (0 == scriptMode.compare("file")) {
               game->getEntity(entityName)->getLuaState()->loadScriptFromFile(script);
            } else {
               game->getEntity(entityName)->getLuaState()->loadScriptFromString(script);
            }
         }

         else if (0 == targetType.compare("class")) {

            std::string className = operation->getChildren()[3]->getValue();

            if (0 == scriptMode.compare("file")) {
               typeClasses[className].get()->getLuaState()->loadScriptFromFile(script);
            } else {
               typeClasses[className].get()->getLuaState()->loadScriptFromString(script);
            }
         }

         else {
            if (0 == scriptMode.compare("file")) {
               game->getLuaState()->loadScriptFromFile(script);
            } else {
               game->getLuaState()->loadScriptFromString(script);
            }
         }
      });

      /**********/

      registerOperation(SET_EVENT, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string event = operation->getChildren()[1]->getValue();
         std::string function = operation->getChildren()[2]->getValue();

         if (0 == targetType.compare("entity")) {

            entity::Entity *entity = game->getEntity(
               operation->getChildren()[3]->getValue()
            ).get();

            entity->getEventListener()->addTrigger(
               event, std::make_unique<event::LuaEventTrigger>(
                  game->err(), function, entity->getLuaState()
               )
            );
         }

         else if (0 == targetType.compare("class")) {

            entity::Entity *entityClass = typeClasses[operation->getChildren()[3]->getValue()].get();

            entityClass->getEventListener()->addTrigger(
               event, std::make_unique<event::LuaEventTrigger>(
                  game->err(), function, entityClass->getLuaState()
               )
            );
         }

         else {
            game->getEventListener()->addTrigger(
               event, std::make_unique<event::LuaEventTrigger>(
                  game->err(), function, game->getLuaState()
               )
            );
         }
      });

      /**********/

      registerOperation(SET_ALIAS, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         entity::Entity *thing = nullptr;

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string alias = operation->getChildren()[1]->getValue();

         if (0 == targetType.compare("entity")) {
            thing = game->getEntity(
               operation->getChildren()[2]->getValue()
            ).get();
         }

         else {
            thing = typeClasses[operation->getChildren()[2]->getValue()].get();
         }

         if (thing) {
            dynamic_cast<entity::Thing *>(thing)->addAlias(alias);
         } else {
            throw UndefinedException("thing = nullptr in Runtime::SET_ALIAS");
         }
      });

      /**********/

      registerOperation(SET_META, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string metaKey = operation->getChildren()[1]->getValue();
         std::string metaValue = operation->getChildren()[2]->getValue();

         if (0 == targetType.compare("entity")) {
            game->getEntity(
               operation->getChildren()[3]->getValue()
            )->setMeta(metaKey, metaValue);
         }

         else if (0 == targetType.compare("class")) {
            typeClasses[operation->getChildren()[3]->getValue()]->setMeta(
               metaKey, metaValue
            );
         }

         else {
            game->setMeta(metaKey, metaValue);
         }
      });

      /**********/

      registerOperation(SET_ATTRIBUTE, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         entity::Entity *being = nullptr;

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string attribute = operation->getChildren()[1]->getValue();
         std::string value = operation->getChildren()[2]->getValue();

         if (0 == targetType.compare("entity")) {
            being = game->getEntity(
               operation->getChildren()[3]->getValue()
            ).get();
         }

         else if (0 == targetType.compare("class")) {
            being = typeClasses[operation->getChildren()[3]->getValue()].get();
         }

         else {
            being = game->getDefaultPlayer();
         }

         if (being) {
            dynamic_cast<entity::Being *>(being)->setAttribute(attribute, stoi(value));
            dynamic_cast<entity::Being *>(being)->setAttributesInitialTotal();
         } else {
            throw UndefinedException("being = nullptr in Runtime::SET_ATTRIBUTE");
         }
      });

      /**********/

      registerOperation(SET_PROPERTY, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         entity::Entity *entity;

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string property = operation->getChildren()[1]->getValue();
         std::string value = operation->getChildren()[2]->getValue();

         if (0 == targetType.compare("entity")) {
            entity = game->getEntity(
               operation->getChildren()[3]->getValue()
            ).get();
            propSetters[entity->getTypeName()][property](game, entity, value);
         }

         else if (0 == targetType.compare("class")) {
            entity = typeClasses[operation->getChildren()[3]->getValue()].get();
            propSetters[entity->getTypeName()][property](game, entity, value);
         }

         else if (0 == targetType.compare("defaultPlayer")) {
            propSetters[game->getDefaultPlayer()->getTypeName()][property](
               game, game->getDefaultPlayer(), value
            );
         }

         else {
            gameSetters[property](game, value);
         }
      });

      /**********/

      registerOperation(INSERT_INTO_INVENTORY, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         const std::shared_ptr<Object> &object = game->getObject(operation->getChildren()[0]->getValue());
         const std::shared_ptr<Being> &owner = game->getBeing(operation->getChildren()[1]->getValue());

         owner->insertIntoInventory(object, false);
      });

      /**********/

      registerOperation(INSERT_INTO_PLACE, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         auto thing = game->getThing(operation->getChildren()[0]->getValue());
         auto room = game->getRoom(operation->getChildren()[1]->getValue());

         room->insertThing(thing);
      });

      /**********/

      registerOperation(CONNECT_ROOMS, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         Room *room;
         auto connectToRoom = game->getRoom(operation->getChildren()[2]->getValue());

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string sourceRoomOrClass = operation->getChildren()[1]->getValue();
         std::string direction = operation->getChildren()[3]->getValue();

         if (0 == targetType.compare("entity")) {
            room = game->getRoom(sourceRoomOrClass).get();
         }

         else {
            room = dynamic_cast<Room *>(typeClasses[sourceRoomOrClass].get());
         }

         room->setConnection(direction, connectToRoom);
      });

      /**********/

      registerOperation(ALLOCATE_RESOURCE, [this]
      (const std::shared_ptr<ASTOperationNode> &operation) {

         std::string entityName = operation->getChildren()[0]->getValue();
         std::string resourceName = operation->getChildren()[1]->getValue();
         std::string amount = operation->getChildren()[2]->getValue();

         if (!isValidDouble(amount)) {
            throw ValidationException("resource allocation: amount must be a valid integer or floating point value");
         }

         auto status = game->getResource(resourceName)->allocate(
            game->getTangible(entityName),
            stod(amount)
         );

         switch (status) {

            case entity::Resource::ALLOCATE_OR_FREE_SUCCESS:
            case entity::Resource::ALLOCATE_OR_FREE_ABORT:
               return;

            case entity::Resource::ALLOCATE_INT_REQUIRED:
               throw ValidationException(
                  std::string("can only allocate '") + resourceName + "' to '" +
                  entityName + "' in integer amounts (line "
                  + std::to_string(operation->getLineNumber()) + ")"
               );

            case entity::Resource::ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED:
               throw ValidationException(
                  std::string("cannot allocate ") + amount + " of '"
                  + resourceName + "' to '" + entityName
                  + "' because it exceeds the amount a tangible entity is allowed to possess (line "
                  + std::to_string(operation->getLineNumber()) + ")"
               );

            case entity::Resource::ALLOCATE_TOTAL_AMOUNT_EXCEEDED:
               throw ValidationException(
                  std::string("cannot allocate ") + amount + " of '"
                  + resourceName + "' to '" + entityName
                  + "' because it exceeds the total amount of the resource that's available (line "
                  + std::to_string(operation->getLineNumber()) + ")"
               );

            case entity::Resource::ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT:
               throw ValidationException(
                  std::string("cannot allocate a zero or negative amount of '")
                  + resourceName + "' to '" + entityName + "' (line "
                  + std::to_string(operation->getLineNumber()) + ")"
               );

            default:
               throw ValidationException(
                  std::string("unknown error occurred when attempting to allocate '")
                  + resourceName + "' to '" + entityName + "' (line "
                  + std::to_string(operation->getLineNumber()) + ")"
               );
         }
      });
   }

   /***************************************************************************/

   // Private method
   void Runtime::mapGameSetters() {

      // If a game introduction is enabled, all players will see this text once
      // before beginning the game
      gameSetters["introduction.text"] = [](Game *game, std::string value) {
         game->setIntroductionText(value);
      };

      /**********/

      // Whether or not a game introduction is enabled
      gameSetters["introduction.enabled"] = [](Game *game, std::string value) {
         game->setIntroductionEnabled(stoi(value));
      };
   }

   /***************************************************************************/

   // Private method
   void Runtime::mapEntitySetters() {

      // Set Entity title (all types)
      propSetters["resource"]["title"] =
      propSetters["room"]["title"] =
      propSetters["object"]["title"] =
      propSetters["creature"]["title"] =
      propSetters["player"]["title"] = [](Game *game, entity::Entity *entity,
      std::string value) {
         entity->setProperty("title", value);
      };

      /**********/

      // Set Entity's long description (all types)
      propSetters["resource"]["longDesc"] =
      propSetters["room"]["longDesc"] =
      propSetters["object"]["longDesc"] =
      propSetters["creature"]["longDesc"] =
      propSetters["player"]["longDesc"] = [](Game *game, entity::Entity *entity,
      std::string value) {
         entity->setProperty("longDesc", value);
      };

      /**********/

      // Set Entity's short description (all types)
      propSetters["resource"]["shortDesc"] =
      propSetters["room"]["shortDesc"] =
      propSetters["object"]["shortDesc"] =
      propSetters["creature"]["shortDesc"] =
      propSetters["player"]["shortDesc"] = [](Game *game, entity::Entity *entity,
      std::string value) {
         entity->setProperty("shortDesc", value);
      };

      /**********/

      // Set a Being's default starting health
      propSetters["creature"]["health"] =
      propSetters["player"]["health"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::HealthProperty,
            stoi(value)
         );
      };

      /**********/

      // Set a Being's maximum health
      propSetters["creature"]["maxhealth"] =
      propSetters["player"]["maxhealth"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::MaxHealthProperty,
            stoi(value)
         );
      };

      /**********/

      // Set the probability that a Being will be wounded when it's attacked
      propSetters["creature"]["woundrate"] =
      propSetters["player"]["woundrate"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::WoundRateProperty,
            stod(value)
         );
      };

      /**********/

      // Set the amount of damage points inflicted by a Being's bare hands during
      // a successful attack
      propSetters["creature"]["damagebarehands"] =
      propSetters["player"]["damagebarehands"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::DamageBareHandsProperty,
            stoi(value)
         );
      };

      /**********/

      // Set whether the Being can respawn
      propSetters["creature"]["respawn.enabled"] =
      propSetters["player"]["respawn.enabled"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::RespawnEnabledProperty,
            static_cast<bool>(stoi(value))
         );
      };

      /**********/

      // Set how long it takes before a Being respawns
      propSetters["creature"]["respawn.interval"] =
      propSetters["player"]["respawn.interval"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::RespawnIntervalProperty,
            stoi(value)
         );
      };

      /**********/

      // Set how many lives a Being has (how many times it can respawn)
      propSetters["creature"]["respawn.lives"] =
      propSetters["player"]["respawn.lives"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::RespawnLivesProperty,
            stoi(value)
         );
      };

      /**********/

      // Set how much total weight a Being's inventory can contain
      propSetters["creature"]["inventory.weight"] =
      propSetters["player"]["inventory.weight"] = [](Game *game, entity::Entity *being,
      std::string value) {
         dynamic_cast<entity::Being *>(being)->setProperty(
            entity::Being::InvMaxWeightProperty,
            stoi(value)
         );
      };

      /**********/

      // Whether or not a Creature will respond to an attack with one of its own
      propSetters["creature"]["counterattack"] = [](Game *game, entity::Entity *creature,
      std::string value) {
         dynamic_cast<entity::Creature *>(creature)->setProperty(
            entity::Creature::CounterAttackProperty,
            static_cast<bool>(stoi(value))
         );
      };

      /**********/

      // Sets the default value of counterattack for a Creature, which depends on
      // its allegiance. This should only be called if the setter for
      // counterattack wasn't called.
      // TODO: I really don't like this property. It'll stay for now, but I should
      // eventually change this with some sort of observer thing where, when
      // the allegiance property is changed, it triggers a re-evaluation of the
      // counterattack (only if counterattack was never set explicitly. Will need
      // to think about the architecture of this...)
      propSetters["creature"]["counterattack.default"] = [](Game *game, entity::Entity *creature,
      std::string value) {

         // By default, Creatures with neutral or enemy allegiances will
         // automatically retaliate when attacked
         switch (dynamic_cast<entity::Creature *>(creature)->getProperty<int>(
            entity::Creature::AllegianceProperty)
         ) {

            case entity::Creature::FRIEND:
               dynamic_cast<entity::Creature *>(creature)->setProperty(
                  entity::Creature::CounterAttackProperty,
                  false
               );
               break;

            default:
               dynamic_cast<entity::Creature *>(creature)->setProperty(
                  entity::Creature::CounterAttackProperty,
                  true
               );
               break;
         }
      };

      /**********/

      // Set a Creature's allegiance
      propSetters["creature"]["allegiance"] = [](Game *game, entity::Entity *creature,
      std::string value) {

         entity::Creature::AllegianceType allegiance = entity::Creature::DEFAULT_ALLEGIANCE;

         if (0 == value.compare("friend")) {
            allegiance = entity::Creature::FRIEND;
         }

         else if (0 == value.compare("neutral")) {
            allegiance = entity::Creature::NEUTRAL;
         }

         else if (0 == value.compare("enemy")) {
            allegiance = entity::Creature::ENEMY;
         }

         else {
            throw ValidationException("allegiance must be 'friend', 'neutral' or 'enemy'");
         }

         dynamic_cast<entity::Creature *>(creature)->setProperty(
            entity::Creature::AllegianceProperty,
            static_cast<entity::Creature::AllegianceType>(allegiance)
         );
      };

      /**********/

      // Whether or not autoattack is enabled for a Creature
      propSetters["creature"]["autoattack.enabled"] = [](Game *game, entity::Entity *creature,
      std::string value) {
         dynamic_cast<entity::Creature *>(creature)->setProperty(
            entity::Creature::AutoAttackEnabledProperty,
            static_cast<bool>(stoi(value))
         );
      };

      /**********/

      // Whether or not a Creature's automatic attack should repeat
      propSetters["creature"]["autoattack.repeat"] = [](Game *game, entity::Entity *creature,
      std::string value) {
         dynamic_cast<entity::Creature *>(creature)->setProperty(
            entity::Creature::AutoAttackRepeatProperty,
            static_cast<bool>(stoi(value))
         );
      };

      /**********/

      // If enabled and repeating, the interval at which a Creature's
      // autoattack is triggered
      propSetters["creature"]["autoattack.interval"] = [](Game *game, entity::Entity *creature,
      std::string value) {
         dynamic_cast<entity::Creature *>(creature)->setProperty(
            entity::Creature::AutoAttackIntervalProperty,
            stoi(value)
         );
      };

      /**********/

      // Whether or not a Creature will wander throughout the game
      propSetters["creature"]["wandering.enabled"] = [](Game *game, entity::Entity *creature,
      std::string value) {
         dynamic_cast<entity::Creature *>(creature)->setWanderEnabled(stoi(value));
      };

      /**********/

      // How many clock ticks pass before the Creature considers wanders again
      propSetters["creature"]["wandering.interval"] = [](Game *game, entity::Entity *creature,
      std::string value) {
         dynamic_cast<entity::Creature *>(creature)->setWanderInterval(stoi(value));
      };

      /**********/

      // The probability that a Creature will actually wander each time it
      // considers moving
      propSetters["creature"]["wandering.wanderlust"] = [](Game *game, entity::Entity *creature,
      std::string value) {
         dynamic_cast<entity::Creature *>(creature)->setWanderLust(stod(value));
      };

      /**********/

      // Set Object's weight
      propSetters["object"]["weight"] = [](Game *game, entity::Entity *object,
      std::string value) {
         dynamic_cast<entity::Object *>(object)->setProperty(
            entity::Object::WeightProperty,
            stoi(value)
         );
      };

      /**********/

      // Set how much damage the Object inflicts if it's a weapon
      propSetters["object"]["damage"] = [](Game *game, entity::Entity *object,
      std::string value) {
         dynamic_cast<entity::Object *>(object)->setProperty(
            entity::Object::DamageProperty,
            stoi(value)
         );
      };

      /**********/

      propSetters["resource"]["amountAvailable"] = [](Game *game, entity::Entity *resource,
      std::string value) {

         if (Entity::PROPERTY_VALID != dynamic_cast<entity::Resource *>(resource)->setProperty(
            entity::Resource::AmtAvailProperty,
            stod(value)
         )) {
            throw ValidationException(
               "Some tangible entity possesses more of the '" +
               resource->getName() + "' than is allowed."
            );
         }
      };

      /**********/

      propSetters["resource"]["requireIntegerAllocations"] = [](Game *game, entity::Entity *resource,
      std::string value) {

         dynamic_cast<entity::Resource *>(resource)->setProperty(
            entity::Resource::ReqIntAllocProperty,
            static_cast<bool>(stoi(value))
         );
      };

      /**********/

      propSetters["resource"]["maxAmountPerDepositor"] = [](Game *game, entity::Entity *resource,
      std::string value) {

         dynamic_cast<entity::Resource *>(resource)->setProperty(
            entity::Resource::MaxAmtPerDepositorProperty,
            stod(value)
         );
      };

      /**********/

      propSetters["resource"]["pluralTitle"] = [](Game *game, entity::Entity *resource,
      std::string value) {

         dynamic_cast<entity::Resource *>(resource)->setProperty(
            entity::Resource::PluralTitleProperty,
            value
         );
      };
   }
}
