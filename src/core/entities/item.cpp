#include "../include/game.h"
#include "../include/entities/item.h"
#include "../include/entities/creature.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   void Item::setIsWeapon(bool w) {

      if (weapon != w) {

         // if a script ever changes an Object to or from a weapon, and
         // it's owned by a Creature, then tell that Creature it has to
         // rebuild its weapons cache so it can properly choose a weapon
         // during combat.
         if (owner != 0 && owner->isType(ENTITY_CREATURE)) {
            static_cast<Creature *>(owner)->clearWeaponCache();
         }

         weapon = w;
      }
   }
}}

