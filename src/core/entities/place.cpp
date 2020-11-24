#include <cmath>
#include <memory>

#include <trogdor/entities/place.h>
#include <trogdor/entities/thing.h>
#include <trogdor/entities/player.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/game.h>
#include <trogdor/iostream/placeout.h>

#include <trogdor/exception/undefinedexception.h>


namespace trogdor::entity {

   Place::Place(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Tangible(g, n, std::move(o), std::move(e)) {

      types.push_back(ENTITY_PLACE);
      static_cast<PlaceOut *>(outStream.get())->setPlace(this);
   }

   /***************************************************************************/

   Place::Place(const Place &p, std::string n): Tangible(p, n) {}

   /***************************************************************************/

   Place::Place(Game *g, const serial::Serializable &data): Tangible(g, data) {

      g->addCallback("afterDeserialize",
      std::make_shared<Entity::EntityCallback>([&](std::any) -> bool {

         std::vector<std::string> serializedThings =
            std::get<std::vector<std::string>>(*data.get("things"));

         for (const auto &thing: serializedThings) {
            if (const std::shared_ptr<Thing> &t = game->getThing(thing)) {
               insertThing(t);
            }
         }

         return true;
      }));

      types.push_back(ENTITY_PLACE);
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Place::serialize() {

      std::shared_ptr<serial::Serializable> data = Tangible::serialize();
      std::vector<std::string> serializedThings;

      for (const auto &thing: things) {
         serializedThings.push_back(thing->getName());
      }

      data->set("things", serializedThings);
      return data;
   }

   /****************************************************************************/

   void Place::insertPlayer(const std::shared_ptr<Player> &player) {

      mutex.lock();

      things.insert(things.end(), player);
      beings.insert(beings.end(), player);
      players.insert(players.end(), player);

      // Index by alias
      for (const auto &alias: player->getAliases()) {
         thingsByName[alias].push_back(player.get());
         beingsByName[alias].push_back(player.get());
         playersByName[alias].push_back(player.get());
      }

      mutex.unlock();
      player->setLocation(getShared());
   }

   /****************************************************************************/

   void Place::insertCreature(const std::shared_ptr<Creature> &creature) {

      mutex.lock();

      things.insert(things.end(), creature);
      beings.insert(beings.end(), creature);
      creatures.insert(creatures.end(), creature);

      // Index by alias
      for (const auto &alias: creature->getAliases()) {
         thingsByName[alias].push_back(creature.get());
         beingsByName[alias].push_back(creature.get());
         creaturesByName[alias].push_back(creature.get());
      }

      mutex.unlock();
      creature->setLocation(getShared());
   }

   /****************************************************************************/

   void Place::insertObject(const std::shared_ptr<Object> &object) {

      mutex.lock();

      things.insert(things.end(), object);
      objects.insert(objects.end(), object);

      // Index by alias
      for (const auto &alias: object->getAliases()) {
         thingsByName[alias].push_back(object.get());
         objectsByName[alias].push_back(object.get());
      }

      mutex.unlock();
      object->setLocation(getShared());
   }

   /****************************************************************************/

   void Place::insertThing(const std::shared_ptr<Thing> &thing) {

      switch (thing->getType()) {

         case ENTITY_PLAYER:
            insertPlayer(std::static_pointer_cast<Player>(thing));
            break;

         case ENTITY_CREATURE:
            insertCreature(std::static_pointer_cast<Creature>(thing));
            break;

         case ENTITY_OBJECT:
            insertObject(std::static_pointer_cast<Object>(thing));
            break;

         default:
            throw UndefinedException(
               std::string("Place::insertThing(): attempting to insert unsupported type '")
               + thing->getTypeName() + "'");
      }
   }

   /****************************************************************************/

   void Place::removePlayer(const std::shared_ptr<Player> &player) {

      for (const auto &alias: player->getAliases()) {

         mutex.lock();

         if (playersByName.find(alias) != playersByName.end()) {
            playersByName.find(alias)->second.remove(player.get());
         }

         if (beingsByName.find(alias) != beingsByName.end()) {
            beingsByName.find(alias)->second.remove(player.get());
         }

         if (thingsByName.find(alias) != thingsByName.end()) {
            thingsByName.find(alias)->second.remove(player.get());
         }

         mutex.unlock();
      }

      mutex.lock();
      players.remove(player);
      beings.remove(player);
      things.remove(player);
      mutex.unlock();

      player->setLocation(std::weak_ptr<Place>());
   }

   /****************************************************************************/

   void Place::removeCreature(const std::shared_ptr<Creature> &creature) {

      for (const auto &alias: creature->getAliases()) {

         mutex.lock();

         if (creaturesByName.find(alias) != creaturesByName.end()) {
            creaturesByName.find(alias)->second.remove(creature.get());
         }

         if (beingsByName.find(alias) != beingsByName.end()) {
            beingsByName.find(alias)->second.remove(creature.get());
         }

         if (thingsByName.find(alias) != thingsByName.end()) {
            thingsByName.find(alias)->second.remove(creature.get());
         }

         mutex.unlock();
      }

      mutex.lock();
      creatures.remove(creature);
      beings.remove(creature);
      things.remove(creature);
      mutex.unlock();

      creature->setLocation(std::weak_ptr<Place>());
   }

   /****************************************************************************/

   void Place::removeObject(const std::shared_ptr<Object> &object) {

      for (const auto &alias: object->getAliases()) {

         mutex.lock();

         if (objectsByName.find(alias) != objectsByName.end()) {
            objectsByName.find(alias)->second.remove(object.get());
         }

         if (thingsByName.find(alias) != thingsByName.end()) {
            thingsByName.find(alias)->second.remove(object.get());
         }

         mutex.unlock();
      }

      mutex.lock();
      objects.remove(object);
      things.remove(object);
      mutex.unlock();

      object->setLocation(std::weak_ptr<Place>());
   }

   /****************************************************************************/

   void Place::removeThing(const std::shared_ptr<Thing> &thing) {

      switch (thing->getType()) {

         case ENTITY_PLAYER:
            removePlayer(std::static_pointer_cast<Player>(thing));
            break;

         case ENTITY_CREATURE:
            removeCreature(std::static_pointer_cast<Creature>(thing));
            break;

         case ENTITY_OBJECT:
            removeObject(std::static_pointer_cast<Object>(thing));
            break;

         default:
            throw UndefinedException(
            std::string("Place::removeThing(): attempting to remove unsupported type '")
               + thing->getTypeName() + "'");
      }
   }

   /****************************************************************************/

   void Place::displayResources(Being *observer) {

      for (const auto &resource: getResources()) {

         auto resourcePtr = resource.first.lock();

         if (resourcePtr) {

            observer->out("display") << std::endl;

            // Display quantity as an integer
            if (resourcePtr->getProperty<bool>(Resource::ReqIntAllocProperty)) {

               if (1 == std::lround(resource.second)) {
                  observer->out("display") << "You see a " <<
                     resourcePtr->getProperty<std::string>(TitleProperty) <<
                     "." << std::endl;
               }

               else {
                  observer->out("display") << "You see " << std::lround(resource.second)
                     << " "
                     << resourcePtr->getProperty<std::string>(Resource::PluralTitleProperty)
                     << "." << std::endl;
               }
            }

            // Display quantity as a double
            else {
               observer->out("display") << "You see " << resource.second << " "
                  << resourcePtr->getProperty<std::string>(Resource::PluralTitleProperty)
                  << "." << std::endl;
            }
         }
      }
   }

   /****************************************************************************/

   void Place::displayThings(Being *observer) {

      for (const auto &thing: things) {

         // Players should see everything except themselves
         if (observer != static_cast<Being *>(thing.get())) {
            observer->out("display") << std::endl;
            thing->glance(observer->getShared());
         }
      }
   }

   /****************************************************************************/

   void Place::displayPlace(Being *observer, bool displayFull) {

      observer->out("location") << getProperty<std::string>(TitleProperty);
      observer->out("location").flush();

      observer->out("display") << getProperty<std::string>(TitleProperty) <<
         std::endl << std::endl;
      Tangible::display(observer, displayFull);
   }

   /****************************************************************************/

   void Place::display(Being *observer, bool displayFull) {

      displayPlace(observer, displayFull);
      displayResources(observer);
      displayThings(observer);
   }
}
