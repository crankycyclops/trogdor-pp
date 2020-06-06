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

   void Place::insertPlayer(Player *player) {

      things.insert(things.end(), player);
      beings.insert(beings.end(), player);
      players.insert(players.end(), player);

      // Index by alias
      for (const auto &alias: player->getAliases()) {
         thingsByName[alias].push_back(player);
         beingsByName[alias].push_back(player);
         playersByName[alias].push_back(player);
      }

      player->setLocation(this);
   }

   /****************************************************************************/

   void Place::insertCreature(Creature *creature) {

      things.insert(things.end(), creature);
      beings.insert(beings.end(), creature);
      creatures.insert(creatures.end(), creature);

      // Index by alias
      for (const auto &alias: creature->getAliases()) {
         thingsByName[alias].push_back(creature);
         beingsByName[alias].push_back(creature);
         creaturesByName[alias].push_back(creature);
      }

      creature->setLocation(this);
   }

   /****************************************************************************/

   void Place::insertObject(Object *object) {

      things.insert(things.end(), object);
      objects.insert(objects.end(), object);

      // Index by alias
      for (const auto &alias: object->getAliases()) {
         thingsByName[alias].push_back(object);
         objectsByName[alias].push_back(object);
      }

      object->setLocation(this);
   }

   /****************************************************************************/

   void Place::insertThing(Thing *thing) {

      switch (thing->getType()) {

         case ENTITY_PLAYER:
            insertPlayer(static_cast<Player *>(thing));
            break;

         case ENTITY_CREATURE:
            insertCreature(static_cast<Creature *>(thing));
            break;

         case ENTITY_OBJECT:
            insertObject(static_cast<Object *>(thing));
            break;

         default:

            throw UndefinedException(
               std::string("Place::insertThing(): attempting to insert unsupported type '")
               + thing->getTypeName() + "'");
      }
   }

   /****************************************************************************/

   void Place::removeThingByName(Thing *thing) {

      std::vector<std::string> aliases = thing->getAliases();

      for (unsigned i = 0; i < aliases.size(); i++) {

         switch (thing->getType()) {

            case ENTITY_PLAYER:

               if (beingsByName.find(aliases[i]) != beingsByName.end()) {
                  beingsByName.find(aliases[i])->second.remove(static_cast<Being *>(thing));
               }

               if (playersByName.find(aliases[i]) != playersByName.end()) {
                  playersByName.find(aliases[i])->second.remove(static_cast<Player *>(thing));
               }

               break;

            case ENTITY_CREATURE:

               if (beingsByName.find(aliases[i]) != beingsByName.end()) {
                  beingsByName.find(aliases[i])->second.remove(static_cast<Being *>(thing));
               }

               if (creaturesByName.find(aliases[i]) != creaturesByName.end()) {
                  creaturesByName.find(aliases[i])->second.remove(static_cast<Creature *>(thing));
               }

               break;

            case ENTITY_OBJECT:

               if (objectsByName.find(aliases[i]) != objectsByName.end()) {
                  objectsByName.find(aliases[i])->second.remove(static_cast<Object *>(thing));
               }

               break;

            default:
               throw UndefinedException(
                  std::string("RemoveThingByName called on unsupported Entity type '")
                  + thing->getTypeName() + "'. This is a bug.");
         }

         if (thingsByName.find(aliases[i]) != thingsByName.end()) {
            thingsByName.find(aliases[i])->second.remove(thing);
         }
      }
   }

   /****************************************************************************/

   void Place::removeThing(Thing *thing) {

      switch (thing->getType()) {

         case ENTITY_PLAYER:

            beings.remove(static_cast<Being *>(thing));
            players.remove(static_cast<Player *>(thing));
            break;

         case ENTITY_CREATURE:

            beings.remove(static_cast<Being *>(thing));
            creatures.remove(static_cast<Creature *>(thing));
            break;

         case ENTITY_OBJECT:

            objects.remove(static_cast<Object *>(thing));
            break;

         default:

            throw UndefinedException(
            std::string("Place::removeThing(): attempting to remove unsupported type '")
               + thing->getTypeName() + "'");
      }

      things.remove(thing);
      removeThingByName(thing);

      thing->setLocation(0);
   }

   /****************************************************************************/

   void Place::display(Being *observer, bool displayFull) {

      observer->out("location") << getTitle();
      observer->out("location").flush();

      observer->out("display") << getTitle() << std::endl << std::endl;
      Entity::display(observer, displayFull);

      for_each(things.begin(), things.end(), [&](Thing *&thing) {
         if (observer != static_cast<Being *>(thing)) { // dirty, but it works
            observer->out("display") << std::endl;
            thing->glance(observer);
         }
      });
   }
}
