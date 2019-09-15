#include <string>
#include <memory>
#include <algorithm>

#include "../../include/game.h"
#include "../../include/utility.h"
#include "../../include/iostream/nullout.h"
#include "../../include/iostream/nullin.h"
#include "../../include/iostream/placeout.h"
#include "../../include/timer/jobs/wander.h"
#include "../../include/instantiator/instantiators/runtime.h"


using namespace std;

namespace trogdor { namespace core {

   Runtime::Runtime(Game *g) {

      game = g;
      mapGameSetters();
      mapEntitySetters();
   }

   /***************************************************************************/

   void Runtime::makeEntityClass(string className, enum entity::EntityType entityType) {

      // Entity classes are represented as model Entity objects that can be copied
      std::unique_ptr<Entity> entity;

      switch (entityType) {

         case entity::ENTITY_ROOM:
            entity = make_unique<Room>(
               game, className, std::make_unique<PlaceOut>(), game->err().clone()
            );
            break;

         case entity::ENTITY_OBJECT:
            entity = make_unique<Object>(
               game, className, std::make_unique<NullOut>(), game->err().clone()
            );
            break;

         case entity::ENTITY_CREATURE:
            // TODO: should Creatures have some kind of special input stream?
            entity = make_unique<Creature>(
               game, className, std::make_unique<NullOut>(), game->err().clone()
            );
            break;

         case entity::ENTITY_PLAYER:
            throw string("Class cannot be defined for type Player");

         default:
            throw string("Class defined for unsupported entity type");
      }

      // for type checking
      entity->setClass(className);
      entity->setTitle(className);

      typeClasses[className] = std::move(entity);
   }

   /***************************************************************************/

   bool Runtime::entityClassExists(string className,
   enum entity::EntityType entityType) {

      // Entity class does not exist
      if (typeClasses.find(className) == typeClasses.end()) {
         return false;
      }

      // Entity class exists, but isn't the correct type
      else if (entityType != typeClasses[className]->getType()) {
         return false;
      }

      // ZOMG CAN HAZ ENTITY CLASS!
      return true;
   }

   /***************************************************************************/

   void Runtime::entityClassSetter(string className, string property,
   string value) {

      if (typeClasses.end() == typeClasses.find(className)) {
         throw string("Entity class does not exist");
      }

      if (propSetters.find(typeClasses[className]->getTypeName()) == propSetters.end()) {
         throw string("Runtime instantiator: entity class: setting property on unsupported entity type");
      }

      else if (propSetters[typeClasses[className]->getTypeName()].find(property) ==
      propSetters[typeClasses[className]->getTypeName()].end()) {
         throw string("Runtime instantiator: entity class: setting unsupported property '") + property + "'";
      }

      propSetters[typeClasses[className]->getTypeName()][property](
         game, typeClasses[className].get(), value
      );
   }

   /***************************************************************************/

   void Runtime::setEntityClassMessage(string className, string messageName,
   string message) {

      if (typeClasses.end() == typeClasses.find(className)) {
         throw string("Entity class does not exist");
      }

      typeClasses[className]->setMessage(messageName, message);
   }

   /***************************************************************************/

   void Runtime::makeEntity(string entityName, enum entity::EntityType entityType,
   string className) {

      // Entity classes are represented as model Entity objects that can be copied
      std::unique_ptr<Entity> entity;

      // Check to see if another entity with the same name already exists
      try {
         Entity *existingEntity = game->getEntity(entityName);
         stringstream s;
         s << "Cannot define " << existingEntity->getTypeName()
            << " '" << entityName << "': an entity with that name already exists";
         throw s.str();
      }

      // In this case, an exception is actually a good thing ;)
      catch (string error) {

         std::shared_ptr<Entity> entity;

         switch (entityType) {

            case entity::ENTITY_ROOM:

               // Entity has no class, so create a blank Entity
               if (
                  0 == className.compare("") ||
                  0 == className.compare(Entity::typeToStr(entity::ENTITY_ROOM))
               ) {
                  entity = std::make_shared<entity::Room>(
                     game, entityName, std::make_unique<PlaceOut>(), game->err().clone()
                 );
               }

               // Entity has a class, so copy the class's prototype
               else {

                  if (!entityClassExists(className, entity::ENTITY_ROOM)) {
                     throw string("room class ") + className + " was not defined";
                  }

                  else {
                     entity = std::make_shared<entity::Room>(
                        *(dynamic_cast<entity::Room *>(typeClasses[className].get())), entityName
                     );
                  }
               }

               break;

            case entity::ENTITY_OBJECT:

               if (
                  0 == className.compare("") ||
                  0 == className.compare(Entity::typeToStr(entity::ENTITY_OBJECT))
               ) {
                  entity = std::make_shared<entity::Object>(
                     game, entityName, std::make_unique<NullOut>(), game->err().clone()
                  );
               }

               else {

                  if (!entityClassExists(className, entity::ENTITY_OBJECT)) {
                     throw string("object class ") + className + " was not defined";
                  }

                  else {
                     entity = make_shared<entity::Object>(
                        *(dynamic_cast<entity::Object *>(typeClasses[className].get())), entityName
                     );
                  }
               }

               break;

            case entity::ENTITY_CREATURE:

               if (
                  0 == className.compare("") ||
                  0 == className.compare(Entity::typeToStr(entity::ENTITY_CREATURE))
               ) {
                  // TODO: should Creatures have some kind of special input stream?
                  entity = make_shared<entity::Creature>(
                     game, entityName, std::make_unique<NullOut>(), game->err().clone()
                  );
               }

               else {

                  if (!entityClassExists(className, entity::ENTITY_CREATURE)) {
                     throw string("creature class ") + className + " was not defined";
                  }

                  else {
                     entity = make_shared<entity::Creature>(
                        *(dynamic_cast<entity::Creature *>(typeClasses[className].get())), entityName
                     );
                  }
               }

               break;

            case entity::ENTITY_PLAYER:
              throw string("TODO: haven't had a reason to instantiate Player objects yet.");

            default:
               throw string("Unsupported entity type. This is a bug.");
         }

         entity->setClass(0 == className.compare("") ? entity->getTypeName() : className);
         game->insertEntity(entityName, entity);
      }
   }

   /***************************************************************************/

   bool Runtime::entityExists(string entityName) {

      try {
         game->getEntity(entityName);
         return true;
      }

      catch (string e) {
         return false;
      }
   }

   /***************************************************************************/

   enum entity::EntityType Runtime::getEntityType(string entityName) {

      return game->getEntity(entityName)->getType();
   }

   /***************************************************************************/

   string Runtime::getEntityClass(string entityName) {

      return game->getEntity(entityName)->getClass();
   }

   /***************************************************************************/

   void Runtime::entitySetter(string entityName, string property,
   string value) {

      entity::Entity *entity;

      try {
         entity = game->getEntity(entityName);
      }

      catch (string e) {
         throw string("Runtime instantiator: entity '") + entityName + "' does not exist. This is a bug.";
      }

      if (propSetters.find(entity->getTypeName()) == propSetters.end()) {
         throw string("Runtime instantiator: setting property on unsupported entity type. This is a bug.");
      }

      else if (propSetters[entity->getTypeName()].find(property) ==
      propSetters[entity->getTypeName()].end()) {
         throw string("Runtime instantiator: setting unsupported property '") + property + ".' This is a bug.";
      }

      propSetters[entity->getTypeName()][property](game, entity, value);
   }

   /***************************************************************************/

   void Runtime::loadEntityScript(string entityName, string script,
   enum LoadScriptMethod method) {

      // TODO
   }

   /***************************************************************************/

   void Runtime::setEntityEvent(string entityName, string eventName,
   string function) {

      // TODO
   }

   /***************************************************************************/

   void Runtime::setEntityMessage(string entityName, string messageName,
   string message) {

      Entity *entity;

      try {
         entity = game->getEntity(entityName);
      }

      catch (string e) {
         throw entityName + "' does not exist";
      }

      entity->setMessage(messageName, message);
   }

   /***************************************************************************/

   void Runtime::defaultPlayerSetter(string property, string value) {

      if (propSetters.find(game->getDefaultPlayer()->getTypeName()) == propSetters.end()) {
         throw string("Runtime instantiator: default player is an unsupported entity type?");
      }

      else if (propSetters[game->getDefaultPlayer()->getTypeName()].find(property) ==
      propSetters[game->getDefaultPlayer()->getTypeName()].end()) {
         throw string("Runtime instantiator: default player: setting unsupported property '") + property + "'";
      }

      propSetters[game->getDefaultPlayer()->getTypeName()][property](
         game, game->getDefaultPlayer().get(), value
      );
   }

   /***************************************************************************/

   void Runtime::setDefaultPlayerMessage(string messageName, string message) {

      game->getDefaultPlayer()->setMessage(messageName, message);
   }

   /***************************************************************************/

   void Runtime::gameSetter(string property, string value) {

      if (gameSetters.find(property) == gameSetters.end()) {
         throw string("Runtime instantiator: unsupported game property '") + property + "'";
      }

      gameSetters[property](game, value);
   }

   /***************************************************************************/

   void Runtime::loadGameScript(string script, enum LoadScriptMethod method) {

      // TODO
   }

   /***************************************************************************/

   void Runtime::setGameEvent(string eventName, string function) {

      // TODO
   }

   /***************************************************************************/

   void Runtime::instantiate() {

      // For each creature, check if wandering was enabled, and if so, insert a
      // timer job for it
      for_each(game->creaturesBegin(), game->creaturesEnd(), [&](auto creatureMapEntry) {

         Creature *creature = dynamic_cast<Creature *>(creatureMapEntry.second.get());

         if (creature->getWanderEnabled()) {
            game->insertTimerJob(std::make_shared<WanderTimerJob>(game, creature->getWanderInterval(),
               -1, creature->getWanderInterval(), creature));
         }
      });
   }

   /***************************************************************************/

   // Private method
   void Runtime::mapGameSetters() {

      gameSetters["introduction.text"] = [](Game *game, string value) {
         game->setIntroductionText(value);
      };

      /**********/

      gameSetters["introduction.enabled"] = [](Game *game, string value) {

         if (value != "1" && value != "0") {
            throw string("game introduction enabled: should be either 1 for true or 0 for false");
         }

         game->setIntroductionEnabled(stoi(value));
      };

      /**********/

      gameSetters["introduction.pause"] = [](Game *game, string value) {

         if (value != "1" && value != "0") {
            throw string("game introduction pause: should be either 1 for true or 0 for false");
         }

         game->setIntroductionPause(stoi(value));
      };

      /**********/

      gameSetters["meta"] = [](Game *game, string value) {

         string metaKey = value.substr(0, value.find(":"));
         string metaValue = value.substr(value.find(":") + 1, value.length());

         game->setMeta(metaKey, metaValue);
      };
   }

   /***************************************************************************/

   // Private method
   void Runtime::mapEntitySetters() {

      // Set Entity title (all types)
      propSetters["room"]["title"] =
      propSetters["object"]["title"] =
      propSetters["creature"]["title"] =
      propSetters["player"]["title"] = [](Game *game, entity::Entity *entity,
      string value) {
         entity->setTitle(value);
      };

      /**********/

      // Set Entity's long description (all types)
      propSetters["room"]["longDesc"] =
      propSetters["object"]["longDesc"] =
      propSetters["creature"]["longDesc"] =
      propSetters["player"]["longDesc"] = [](Game *game, entity::Entity *entity,
      string value) {
         entity->setLongDescription(value);
      };

      /**********/

      // Set Entity's short description (all types)
      propSetters["room"]["shortDesc"] =
      propSetters["object"]["shortDesc"] =
      propSetters["creature"]["shortDesc"] =
      propSetters["player"]["shortDesc"] = [](Game *game, entity::Entity *entity,
      string value) {
         entity->setShortDescription(value);
      };

      /**********/

      // Set meta value for Entity (all types)
      propSetters["room"]["meta"] =
      propSetters["object"]["meta"] =
      propSetters["creature"]["meta"] =
      propSetters["player"]["meta"] = [](Game *game, entity::Entity *entity,
      string value) {

         string metaKey = value.substr(0, value.find(":"));
         string metaValue = value.substr(value.find(":") + 1, value.length());

         entity->setMeta(metaKey, metaValue);
      };

      /**********/

      // Set an alias for a Thing (all types that inherit from Thing)
      propSetters["object"]["alias"] =
      propSetters["creature"]["alias"] =
      propSetters["player"]["alias"] = [](Game *game, entity::Entity *thing,
      string value) {
         dynamic_cast<entity::Thing *>(thing)->addAlias(value);
      };

      /**********/

      // Set whether or not a Being is alive
      propSetters["creature"]["alive"] =
      propSetters["player"]["alive"] = [](Game *game, entity::Entity *being,
      string value) {

         if (value != "1" && value != "0") {
            throw string("being alive: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Being *>(being)->setAlive(stoi(value));
      };

      /**********/

      // Set a Being's default starting health
      propSetters["creature"]["health"] =
      propSetters["player"]["health"] = [](Game *game, entity::Entity *being,
      string value) {

         if (!isValidInteger(value)) {
            throw string("being health: should be a valid integer");
         }

         dynamic_cast<entity::Being *>(being)->setHealth(stoi(value));
      };

      /**********/

      // Set a Being's maximum health
      propSetters["creature"]["maxhealth"] =
      propSetters["player"]["maxhealth"] = [](Game *game, entity::Entity *being,
      string value) {

         if (!isValidInteger(value)) {
            throw string("being max health: should be a valid integer");
         }

         dynamic_cast<entity::Being *>(being)->setMaxHealth(stoi(value));
      };

      /**********/

      // Set whether or not a Being is attackable
      propSetters["creature"]["attackable"] =
      propSetters["player"]["attackable"] = [](Game *game, entity::Entity *being,
      string value) {

         if (value != "1" && value != "0") {
            throw string("being attackable: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Being *>(being)->setAttackable(stoi(value));
      };

      /**********/

      // Set the probability that a Being will be wounded when it's attacked
      propSetters["creature"]["woundrate"] =
      propSetters["player"]["woundrate"] = [](Game *game, entity::Entity *being,
      string value) {

         if (!isValidDouble(value)) {
            throw string("being wound rate: should be a valid decimal probability between 0 and 1");
         }

         dynamic_cast<entity::Being *>(being)->setWoundRate(stod(value));
      };

      /**********/

      // Set the amount of damage points inflicted by a Being's bare hands during
      // a successful attack
      propSetters["creature"]["damagebarehands"] =
      propSetters["player"]["damagebarehands"] = [](Game *game, entity::Entity *being,
      string value) {

         if (!isValidInteger(value)) {
            throw string("being damagebarehands: should be a valid integer");
         }

         dynamic_cast<entity::Being *>(being)->setDamageBareHands(stoi(value));
      };

      /**********/

      // Set whether the Being can respawn
      propSetters["creature"]["respawn.enabled"] =
      propSetters["player"]["respawn.enabled"] = [](Game *game, entity::Entity *being,
      string value) {

         if (value != "1" && value != "0") {
            throw string("being respawn: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Being *>(being)->setRespawnEnabled(stoi(value));
      };

      /**********/

      // Set how long it takes before a Being respawns
      propSetters["creature"]["respawn.interval"] =
      propSetters["player"]["respawn.interval"] = [](Game *game, entity::Entity *being,
      string value) {

         if (!isValidInteger(value)) {
            throw string("being respawn interval: Expected integer value");
         }

         dynamic_cast<entity::Being *>(being)->setRespawnInterval(stoi(value));
      };

      /**********/

      // Set how many lives a Being has (how many times it can respawn)
      propSetters["creature"]["respawn.lives"] =
      propSetters["player"]["respawn.lives"] = [](Game *game, entity::Entity *being,
      string value) {

         if (!isValidInteger(value)) {
            throw string("being respawn lives: Expected integer value");
         }

         dynamic_cast<entity::Being *>(being)->setRespawnLives(stoi(value));
      };

      /**********/

      // Set how much total weight a Being's inventory can contain
      propSetters["creature"]["inventory.weight"] =
      propSetters["player"]["inventory.weight"] = [](Game *game, entity::Entity *being,
      string value) {

         if (!isValidInteger(value)) {
            throw string("being inventory weight: Expected integer value");
         }

         dynamic_cast<entity::Being *>(being)->setInventoryWeight(stoi(value));
      };

      /**********/

      // Inserts an Object into a Being's inventory
      propSetters["creature"]["inventory.object"] =
      propSetters["player"]["inventory.object"] = [](Game *game, entity::Entity *being,
      string value) {

         Object *object;

         try {
            object = game->getObject(value);
         }

         catch (string error) {
            throw string("object '") + value + "' doesn't exist";
         }

         if (0 != object->getOwner()) {
            throw string("object '") + value + "' is already owned by '"
               + object->getOwner()->getName();
         }

         else if (0 != object->getLocation()) {
            throw string("object '") + value + "' was already placed "
               + "in room '" + object->getLocation()->getName();
         }

         dynamic_cast<entity::Being *>(being)->insertIntoInventory(object, false);
      };

      /**********/

      // Set a Being's attribute
      propSetters["creature"]["attribute"] =
      propSetters["player"]["attribute"] = [](Game *game, entity::Entity *being,
      string value) {

         string attr = value.substr(0, value.find(":"));
         string attrValue = value.substr(value.find(":") + 1, value.length());

         if (!isValidInteger(attrValue)) {
            throw string("being attribute: Expected integer value");
         }

         dynamic_cast<entity::Being *>(being)->setAttribute(attr, stoi(attrValue));
         dynamic_cast<entity::Being *>(being)->setAttributesInitialTotal();
      };

      /**********/

      // Whether or not a Creature will respond to an attack with one of its own
      propSetters["creature"]["counterattack"] = [](Game *game, entity::Entity *creature,
      string value) {

         if (value != "1" && value != "0") {
            throw string("creature counter attack: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Creature *>(creature)->setCounterAttack(stoi(value));
      };

      /**********/

      // Sets the default value of counterattack for a Creature, which depends on
      // its allegiance. This should only be called if the setter for
      // counterattack wasn't called.
      propSetters["creature"]["counterattack.default"] = [](Game *game, entity::Entity *creature,
      string value) {

         // By default, Creatures with neutral or enemy allegiances will
         // automatically retaliate when attacked
         switch (dynamic_cast<entity::Creature *>(creature)->getAllegiance()) {

            case entity::Creature::FRIEND:
               dynamic_cast<entity::Creature *>(creature)->setCounterAttack(false);
               break;

            default:
               dynamic_cast<entity::Creature *>(creature)->setCounterAttack(true);
               break;
         }
      };

      /**********/

      // Set a Creature's allegiance
      propSetters["creature"]["allegiance"] = [](Game *game, entity::Entity *creature,
      string value) {

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
            throw string("creature allegiance: must be 'friend', 'neutral' or 'enemy'");
         }

         dynamic_cast<entity::Creature *>(creature)->setAllegiance(allegiance);
      };

      /**********/

      // Whether or not autoattack is enabled for a Creature
      propSetters["creature"]["autoattack.enabled"] = [](Game *game, entity::Entity *creature,
      string value) {

         if (value != "1" && value != "0") {
            throw string("creature autoattack enabled: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Creature *>(creature)->setAutoAttackEnabled(stoi(value));
      };

      /**********/

      // Whether or not a Creature's automatic attack should repeat
      propSetters["creature"]["autoattack.repeat"] = [](Game *game, entity::Entity *creature,
      string value) {

         if (value != "1" && value != "0") {
            throw string("object takeable: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Creature *>(creature)->setAutoAttackRepeat(stoi(value));
      };

      /**********/

      // If enabled and repeating, the interval at which a Creature's
      // autoattack is triggered
      propSetters["creature"]["autoattack.interval"] = [](Game *game, entity::Entity *creature,
      string value) {

         if (!isValidInteger(value)) {
            throw string("object damage: Expected integer value");
         }

         dynamic_cast<entity::Creature *>(creature)->setAutoAttackInterval(stoi(value));
      };

      /**********/

      // Whether or not to enable Creature wandering
      propSetters["creature"]["wandering.enabled"] = [](Game *game, entity::Entity *creature,
      string value) {

         if (value != "1" && value != "0") {
            throw string("creature wandering enable: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Creature *>(creature)->setWanderEnabled(stoi(value));
      };

      /**********/

      // How many clock ticks pass before the Creature considers wanders again
      propSetters["creature"]["wandering.interval"] = [](Game *game, entity::Entity *creature,
      string value) {

         if (!isValidInteger(value)) {
            throw string("creature wandering interval: Expected integer value");
         }

         dynamic_cast<entity::Creature *>(creature)->setWanderInterval(stoi(value));
      };

      /**********/

      // The probability that a Creature will actually wander each time it
      // considers moving
      propSetters["creature"]["wandering.wanderlust"] = [](Game *game, entity::Entity *creature,
      string value) {

         if (!isValidDouble(value)) {
            throw string("creature wanderlust: should be a valid decimal probability between 0 and 1");
         }

         dynamic_cast<entity::Creature *>(creature)->setWanderLust(stod(value));
      };

      /**********/

      // Connect one room to another
      propSetters["room"]["connection"] = [](Game *game, entity::Entity *room,
      string value) {

         Room *connectToRoom;

         string direction = value.substr(0, value.find(":"));
         string connectToName = value.substr(value.find(":") + 1, value.length());

         try {
            connectToRoom = game->getRoom(connectToName);
         }

         catch (string e) {
            throw string ("setting ") + room->getName() + "->" + value
               + ": room '" + value + "' does not exist";
         }

         dynamic_cast<entity::Room *>(room)->setConnection(direction, connectToRoom);
      };

      /**********/

      // Insert a Thing into a room
      propSetters["room"]["contains"] = [](Game *game, entity::Entity *room,
      string value) {

         entity::Thing *thing;

         try {
            thing = game->getThing(value);
         }

         catch (string error) {
            throw string("Thing '") + value + "' doesn't exist";
         }

         dynamic_cast<entity::Room *>(room)->insertThing(thing);
      };

      /**********/

      // Set Object's weight
      propSetters["object"]["weight"] = [](Game *game, entity::Entity *object,
      string value) {

         if (!isValidInteger(value)) {
            throw string("object weight: Expected integer value");
         }

         dynamic_cast<entity::Object *>(object)->setWeight(stoi(value));
      };

      /**********/

      // Set whether the Object can be taken
      propSetters["object"]["takeable"] = [](Game *game, entity::Entity *object,
      string value) {

         if (value != "1" && value != "0") {
            throw string("object takeable: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Object *>(object)->setTakeable(stoi(value));
      };

      /**********/

      // Set whether the Object can be dropped
      propSetters["object"]["droppable"] = [](Game *game, entity::Entity *object,
      string value) {

         if (value != "1" && value != "0") {
            throw string("object droppable: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Object *>(object)->setDroppable(stoi(value));
      };

      /**********/

      // Set whether or not the Object is a weapon
      propSetters["object"]["weapon"] = [](Game *game, entity::Entity *object,
      string value) {

         if (value != "1" && value != "0") {
            throw string("object weapon: should be either 1 for true or 0 for false");
         }

         dynamic_cast<entity::Object *>(object)->setIsWeapon(stoi(value));
      };

      /**********/

      // Set how much damage the Object inflicts if it's a weapon
      propSetters["object"]["damage"] = [](Game *game, entity::Entity *object,
      string value) {

         if (!isValidInteger(value)) {
            throw string("object damage: Expected integer value");
         }

         dynamic_cast<entity::Object *>(object)->setDamage(stoi(value));
      };
   }
}}

