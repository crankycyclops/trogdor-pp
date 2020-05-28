#include <random>

#include <trogdor/game.h>
#include <trogdor/entities/creature.h>


namespace trogdor::entity {


   Creature::Creature(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Being(g, n, std::move(o),
   std::make_unique<NullIn>(), std::move(e)),
   counterAttack(DEFAULT_COUNTER_ATTACK), allegiance(DEFAULT_ALLEGIANCE) {

      types.push_back(ENTITY_CREATURE);
      setClass("creature");

      autoAttack.enabled = DEFAULT_AUTO_ATTACK_ENABLED;
      autoAttack.interval = DEFAULT_AUTO_ATTACK_INTERVAL;
      autoAttack.repeat = DEFAULT_AUTO_ATTACK_REPEAT;
   }

   /***************************************************************************/

   Creature::Creature(const Creature &c, std::string n): Being(c, n) {

      autoAttack = c.autoAttack;
      wanderSettings = c.wanderSettings;
      counterAttack = c.counterAttack;
      allegiance = c.allegiance;
   }

   /***************************************************************************/

   // TODO: I can reimplement this as a BST so I don't have to rebuilt the
   // whole list each time a weapon is added or removed from an inventory
   void Creature::buildWeaponCache() {

      for (auto const &object: getInventoryObjects()) {

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
      };
   }

   /***************************************************************************/

   Object *Creature::selectWeapon() {

      static std::random_device rd;
      static std::mt19937 generator(rd());
      static std::uniform_real_distribution<double> distribution(0, 1);

      // calculate probability that Creature will use a weapon
      double p = getAttributeFactor("dexterity") * 0.8;
      p = p > 0.0 ? p + 0.2 : 0.0;

      // creature was able to use a weapon
      if (distribution(generator) < p) {

         if (0 == weaponCache.size()) {
            buildWeaponCache();
         }

         double pSelectWeapon = getAttributeFactor("intelligence") * 0.8;
         pSelectWeapon = pSelectWeapon > 0.0 ? pSelectWeapon + 0.2 : 0.0;

         // select the next strongest weapon with some probability determined by
         // intelligence
         for (auto &weapon: weaponCache) {
            if (distribution(generator) < pSelectWeapon) {
               return weapon;
            }
         }

         // we got all the way to the end, so return the weakest weapon
         return weaponCache.back();
      }

      // creature either didn't have a weapon or couldn't use one
      return 0;
   }
}
