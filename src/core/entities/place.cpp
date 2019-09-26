#include <algorithm>
#include <memory>

#include "../include/entities/place.h"
#include "../include/entities/thing.h"
#include "../include/entities/player.h"
#include "../include/entities/creature.h"
#include "../include/entities/object.h"

#include "../include/game.h"
#include "../include/iostream/nullin.h"
#include "../include/iostream/placeout.h"

#include "../include/exception/undefinedexception.h"


using namespace std;

namespace trogdor { namespace entity {

   Place::Place(Game *g, string n, std::unique_ptr<Trogout> o, std::unique_ptr<Trogout> e):
   Entity(g, n, std::move(o), std::move(std::make_unique<NullIn>()), std::move(e)) {

      types.push_back(ENTITY_PLACE);
      static_cast<PlaceOut *>(outStream.get())->setPlace(this);
   }

   /***************************************************************************/

   void Place::insertThingByName(Thing *thing) {

      vector<string> aliases = thing->getAliases();

      for (int i = aliases.size() - 1; i >= 0; i--) {
         indexThingName(aliases[i], thing);
      }
   }

   /****************************************************************************/

   void Place::insertThingByName(Being *being) {

      vector<string> aliases = being->getAliases();

      for (int i = aliases.size() - 1; i >= 0; i--) {
         indexBeingName(aliases[i], being);
      }
   }

   /****************************************************************************/

   void Place::insertThingByName(Player *player) {

      vector<string> aliases = player->getAliases();

      for (int i = aliases.size() - 1; i >= 0; i--) {

         if (playersByName.find(aliases[i]) == playersByName.end()) {
            PlayerList newList;
            playersByName[aliases[i]] = newList;
         }

         playersByName.find(aliases[i])->second.push_back(player);
      }
   }

   /****************************************************************************/

   void Place::insertThingByName(Creature *creature) {

      vector<string> aliases = creature->getAliases();

      for (int i = aliases.size() - 1; i >= 0; i--) {

         if (creaturesByName.find(aliases[i]) == creaturesByName.end()) {
            CreatureList newList;
            creaturesByName[aliases[i]] = newList;
         }

         creaturesByName.find(aliases[i])->second.push_back(creature);
      }
   }

   /****************************************************************************/

   void Place::insertThingByName(Object *object) {

      vector<string> aliases = object->getAliases();

      for (int i = aliases.size() - 1; i >= 0; i--) {

         if (objectsByName.find(aliases[i]) == objectsByName.end()) {
            ObjectList newList;
            objectsByName[aliases[i]] = newList;
         }

         objectsByName.find(aliases[i])->second.push_back(object);
      }
   }

   /****************************************************************************/

   void Place::insertThing(Thing *thing) {

      switch (thing->getType()) {

         case ENTITY_PLAYER:

            beings.insert(beings.end(), static_cast<Being *>(thing));
            players.insert(players.end(), static_cast<Player *>(thing));
            insertThingByName(static_cast<Being *>(thing));
            insertThingByName(static_cast<Player *>(thing));
            break;

         case ENTITY_CREATURE:

            beings.insert(beings.end(), static_cast<Being *>(thing));
            creatures.insert(creatures.end(), static_cast<Creature *>(thing));
            insertThingByName(static_cast<Being *>(thing));
            insertThingByName(static_cast<Creature *>(thing));
            break;

         case ENTITY_OBJECT:

            objects.insert(objects.end(), static_cast<Object *>(thing));
            insertThingByName(static_cast<Object *>(thing));
            break;

         default:

            throw UndefinedException(
               string("Place::insertThing(): attempting to insert unsupported type '")
               + thing->getTypeName() + "'");
      }

      things.insert(things.end(), thing);
      insertThingByName(thing);

      // Things require a reference to the containing Place
      thing->setLocation(this);
   }

   /****************************************************************************/

   void Place::removeThingByName(Thing *thing) {

      vector<string> aliases = thing->getAliases();

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
                  string("RemoveThingByName called on unsupported Entity type '")
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
            string("Place::removeThing(): attempting to remove unsupported type '")
               + thing->getTypeName() + "'");
      }

      things.remove(thing);
      removeThingByName(thing);

      thing->setLocation(0);
   }

   /****************************************************************************/

   void Place::display(Being *observer, bool displayFull) {

      observer->out("display") << getTitle() << endl << endl;
      Entity::display(observer, displayFull);

      for_each(things.begin(), things.end(), [&](Thing *&thing) {
         if (observer != static_cast<Being *>(thing)) { // dirty, but it works
            observer->out("display") << endl;
            thing->glance(observer);
         }
      });
   }
}}

