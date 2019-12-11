#include <algorithm>

#include "../include/game.h"
#include "../include/entities/creature.h"


namespace trogdor { namespace entity {


   // TODO: I can reimplement this as a BST so I don't have to rebuilt the
   // whole list each time a weapon is added or removed from an inventory
   void Creature::buildWeaponCache() {

      ObjectSetCItPair objs = getInventoryObjects();

      for_each(objs.begin, objs.end, [&](Object * const &object) {

         if (object->isTagSet(Object::WeaponTag)) {

            bool inserted = false;

            // insert weapon into the cache in sorted order (can't use for_each
            // here because I need the iterator to insert at the correct position)
            for (auto i = weaponCache.begin(); i != weaponCache.end(); i++) {
               if (object->getDamage() >= (*i)->getDamage()) {
                  weaponCache.insert(i, object);
                  inserted = true;
                  break;
               }
            }

            // object belongs at the very back
            if (!inserted) {
               weaponCache.push_back(object);
            }
         }
      });
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
         for (auto &weapon: weaponCache) {
            if (d.roll() < pSelectWeapon) {
               return weapon;
            }
         }

         // we got all the way to the end, so return the weakest weapon
         return weaponCache.back();
      }

      // creature either didn't have a weapon or couldn't use one
      return 0;
   }
}}

