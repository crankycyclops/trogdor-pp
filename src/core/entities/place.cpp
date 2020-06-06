#include <algorithm>
#include <memory>

#include <trogdor/entities/place.h>
#include <trogdor/entities/thing.h>
#include <trogdor/entities/player.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/game.h>
#include <trogdor/iostream/nullin.h>
#include <trogdor/iostream/placeout.h>

#include <trogdor/exception/undefinedexception.h>


namespace trogdor::entity {

   Place::Place(Game *g, std::string n, std::unique_ptr<Trogout> o, std::unique_ptr<Trogerr> e):
   Entity(g, n, std::move(o), std::make_unique<NullIn>(), std::move(e)) {

      types.push_back(ENTITY_PLACE);
      static_cast<PlaceOut *>(outStream.get())->setPlace(this);
   }

   /***************************************************************************/

   Place::Place(const Place &p, std::string n): Entity(p, n) {

   }

   /****************************************************************************/

   void Place::insertPlayer(const std::shared_ptr<Player> &player) {

      things.insert(things.end(), player);
      beings.insert(beings.end(), player);
      players.insert(players.end(), player);

      // Index by alias
      for (const auto &alias: player->getAliases()) {
         thingsByName[alias].push_back(player.get());
         beingsByName[alias].push_back(player.get());
         playersByName[alias].push_back(player.get());
      }

      player->setLocation(getShared());
   }

   /****************************************************************************/

   void Place::insertCreature(const std::shared_ptr<Creature> &creature) {

      things.insert(things.end(), creature);
      beings.insert(beings.end(), creature);
      creatures.insert(creatures.end(), creature);

      // Index by alias
      for (const auto &alias: creature->getAliases()) {
         thingsByName[alias].push_back(creature.get());
         beingsByName[alias].push_back(creature.get());
         creaturesByName[alias].push_back(creature.get());
      }

      creature->setLocation(getShared());
   }

   /****************************************************************************/

   void Place::insertObject(const std::shared_ptr<Object> &object) {

      things.insert(things.end(), object);
      objects.insert(objects.end(), object);

      // Index by alias
      for (const auto &alias: object->getAliases()) {
         thingsByName[alias].push_back(object.get());
         objectsByName[alias].push_back(object.get());
      }

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

         if (playersByName.find(alias) != playersByName.end()) {
            playersByName.find(alias)->second.remove(player.get());
         }

         if (beingsByName.find(alias) != beingsByName.end()) {
            beingsByName.find(alias)->second.remove(player.get());
         }

         if (thingsByName.find(alias) != thingsByName.end()) {
            thingsByName.find(alias)->second.remove(player.get());
         }
      }

      players.remove(player);
      beings.remove(player);
      things.remove(player);

      player->setLocation(std::weak_ptr<Place>());
   }

   /****************************************************************************/

   void Place::removeCreature(const std::shared_ptr<Creature> &creature) {

      for (const auto &alias: creature->getAliases()) {

         if (creaturesByName.find(alias) != creaturesByName.end()) {
            creaturesByName.find(alias)->second.remove(creature.get());
         }

         if (beingsByName.find(alias) != beingsByName.end()) {
            beingsByName.find(alias)->second.remove(creature.get());
         }

         if (thingsByName.find(alias) != thingsByName.end()) {
            thingsByName.find(alias)->second.remove(creature.get());
         }
      }

      creatures.remove(creature);
      beings.remove(creature);
      things.remove(creature);

      creature->setLocation(std::weak_ptr<Place>());
   }

   /****************************************************************************/

   void Place::removeObject(const std::shared_ptr<Object> &object) {

      for (const auto &alias: object->getAliases()) {

         if (objectsByName.find(alias) != objectsByName.end()) {
            objectsByName.find(alias)->second.remove(object.get());
         }

         if (thingsByName.find(alias) != thingsByName.end()) {
            thingsByName.find(alias)->second.remove(object.get());
         }
      }

      objects.remove(object);
      things.remove(object);

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

   void Place::display(Being *observer, bool displayFull) {

      observer->out("location") << getTitle();
      observer->out("location").flush();

      observer->out("display") << getTitle() << std::endl << std::endl;
      Entity::display(observer, displayFull);

      for_each(things.begin(), things.end(), [&](const std::shared_ptr<Thing> &thing) {
         if (observer != static_cast<Being *>(thing.get())) { // dirty, but it works
            observer->out("display") << std::endl;
            thing->glance(observer);
         }
      });
   }
}
