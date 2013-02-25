#include "../include/entities/place.h"
#include "../include/entities/thing.h"
#include "../include/entities/player.h"
#include "../include/entities/creature.h"
#include "../include/entities/object.h"


using namespace std;

namespace core { namespace entity {


   void Place::insertThing(Thing *thing) {


      switch (thing->getType()) {

         ENTITY_PLAYER:
            beings.insert(beings.end(), static_cast<Being *>(thing));
            players.insert(players.end(), static_cast<Player *>(thing));
            // TODO: insert by name, including synonyms
            break;

         ENTITY_CREATURE:
            beings.insert(beings.end(), static_cast<Being *>(thing));
            creatures.insert(creatures.end(), static_cast<Creature *>(thing));
            // TODO: insert by name, including synonyms
            break;

         ENTITY_OBJECT:
            items.insert(items.end(), static_cast<Item *>(thing));
            objects.insert(objects.end(), static_cast<Object *>(thing));
            // TODO: insert by name, including synonyms
            break;

         default:
            break;
      }

      things.insert(things.end(), thing);
      // TODO: insert by name, including synonyms
   }
}}

