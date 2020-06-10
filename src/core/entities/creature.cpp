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

   bool Creature::insertIntoInventory(
      const std::shared_ptr<Object> &object,
      bool considerWeight
   ) {

      if (Being::insertIntoInventory(object, considerWeight)) {

         // Make sure we initialize the callback that will be used to update the
         // Creature's weapon cache every time the weapon tag is added or removed
         // from an Object in the Creature's inventory.
         if (!updateObjectTag) {
            updateObjectTag = std::make_shared<std::function<void(std::any)>>([&](std::any data) {

               auto args = std::any_cast<std::tuple<std::string, Object *>>(data);

               std::string tag = std::get<0>(args);
               Object *object = static_cast<Object *>(std::get<1>(args));

               if (0 == tag.compare(Object::WeaponTag)) {

                  // The weapon tag was added
                  if (object->isTagSet(Object::WeaponTag)) {
                     weaponCache.insert(object);
                  }

                  // The weapon tag was removed
                  else {
                     weaponCache.erase(object);
                  }
               }
            });
         }

         if (object->isTagSet(Object::WeaponTag)) {
            weaponCache.insert(object.get());
            object->addCallback("setTag", updateObjectTag);
            object->addCallback("removeTag", updateObjectTag);
         }

         return true;
      }

      return false;
   }

   /***************************************************************************/

   void Creature::removeFromInventory(const std::shared_ptr<Object> &object) {

      if (updateObjectTag) {
         object->removeCallback("setTag", updateObjectTag);
         object->removeCallback("removeTag", updateObjectTag);
      }

      if (object->isTagSet(Object::WeaponTag)) {
         weaponCache.erase(object.get());
      }

      Being::removeFromInventory(object);
   }

   /***************************************************************************/

   Object *Creature::selectWeapon() {

      // Creature doesn't have any weapons
      if (!weaponCache.size()) {
         return nullptr;
      }

      static std::random_device rd;
      static std::mt19937 generator(rd());
      static std::uniform_real_distribution<double> distribution(0, 1);

      // calculate probability that Creature will use a weapon
      double p = getAttributeFactor("dexterity") * 0.8;
      p = p > 0.0 ? p + 0.2 : 0.0;

      // creature was able to use a weapon
      if (distribution(generator) < p) {

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
         return *weaponCache.rbegin();
      }

      // creature either didn't have a weapon or couldn't use one
      return nullptr;
   }
}
