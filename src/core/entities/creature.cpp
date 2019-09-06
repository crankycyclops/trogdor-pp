#include "../include/game.h"
#include "../include/entities/creature.h"


using namespace std;
using namespace boost;

namespace trogdor { namespace core { namespace entity {


   void Creature::buildWeaponCache() {

      ObjectSetCItPair objs = getInventoryObjects();

      for (ObjectSetCIt o = objs.begin; o != objs.end; o++) {

         if ((*o)->isWeapon()) {

            bool inserted = false;

            // insert weapon into the cache in sorted order
            for (ObjectListIt i = weaponCache.begin(); i != weaponCache.end(); i++) {
               if ((*o)->getDamage() >= (*i)->getDamage()) {
                  weaponCache.insert(i, *o);
                  inserted = true;
                  break;
               }
            }

            // object belongs at the very back
            if (!inserted) {
               weaponCache.push_back(*o);
            }
         }
      }
   }

   Object *Creature::selectWeapon() {

      Dice d;

      // calculate probability that Creature will use a weapon
      double p = getAttributeFactor("dexterity") * 0.8;
      p = p > 0.0 ? p + 0.2 : 0.0;

      // creature was able to use a weapon
      if (d.get() < p) {

         if (0 == weaponCache.size()) {
            buildWeaponCache();
         }

         double pSelectWeapon = getAttributeFactor("intelligence") * 0.8;
         pSelectWeapon = pSelectWeapon > 0.0 ? pSelectWeapon + 0.2 : 0.0;

         // select the next strongest weapon with some probability determined by
         // intelligence
         for (ObjectListCIt i = weaponCache.begin(); i != weaponCache.end(); i++) {
            if (d.roll() < pSelectWeapon) {
               return *i;
            }
         }

         // we got all the way to the end, so return the weakest weapon
         return weaponCache.back();
      }

      // creature either didn't have a weapon or couldn't use one
      return 0;
   }
}}}

