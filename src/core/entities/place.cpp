#include "../include/entities/place.h"
#include "../include/entities/thing.h"
#include "../include/entities/player.h"
#include "../include/entities/creature.h"
#include "../include/entities/object.h"

#include "../include/game.h"


using namespace std;

namespace core { namespace entity {


   void Place::insertThingByName(Thing *thing) {

      vector<string> aliases = thing->getAliases();

      for (int i = aliases.size() - 1; i >= 0; i--) {

         if (thingsByName.find(aliases[i]) == thingsByName.end()) {
            ThingList newList;
            thingsByName[aliases[i]] = newList;
         }

         thingsByName.find(aliases[i])->second.push_back(thing);
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
            insertThingByName(static_cast<Player *>(thing));
            break;

         case ENTITY_CREATURE:

            beings.insert(beings.end(), static_cast<Being *>(thing));
            creatures.insert(creatures.end(), static_cast<Creature *>(thing));
            insertThingByName(static_cast<Creature *>(thing));
            break;

         case ENTITY_OBJECT:

            items.insert(items.end(), static_cast<Item *>(thing));
            objects.insert(objects.end(), static_cast<Object *>(thing));
            insertThingByName(static_cast<Object *>(thing));
            break;

         default:

            stringstream s;
            s << "Place::insertThing(): attempting to insert unsupported type "
               << thing->getType();
            throw s.str();
      }

      things.insert(things.end(), thing);
      insertThingByName(thing);

      // Things require a reference to the containing Place
      thing->setLocation(this);
   }

   /****************************************************************************/

   void Place::display(Being *observer) {

      *game->trogout << getTitle() << endl;
      Entity::display(observer);

      for (ThingList::iterator i = things.begin(); i != things.end(); i++) {
         (*i)->glance(observer);
         *game->trogout << endl;
      }
   }
}}

