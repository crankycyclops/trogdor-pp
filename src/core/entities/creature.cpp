#include <random>

#include <trogdor/game.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/room.h>


namespace trogdor::entity {


   void Creature::setPropertyValidators() {

      setPropertyValidator(AutoAttackEnabledProperty, [&](PropertyValue v) -> int {return isPropertyValueBool(v);});
      setPropertyValidator(AutoAttackRepeatProperty, [&](PropertyValue v) -> int {return isPropertyValueBool(v);});
      setPropertyValidator(AutoAttackIntervalProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});

      setPropertyValidator(CounterAttackProperty, [&](PropertyValue v) -> int {return isPropertyValueBool(v);});
      setPropertyValidator(WanderEnabledProperty, [&](PropertyValue v) -> int {return isPropertyValueBool(v);});

      setPropertyValidator(WanderIntervalProperty, [&](PropertyValue v) -> int {

         if (PROPERTY_VALID != isPropertyValueInt(v)) {
            return PROPERTY_INVALID_TYPE;
         }

         if (std::get<int>(v) < 1) {
            return WANDER_INTERVAL_LESS_THAN_ONE;
         }

         return PROPERTY_VALID;
      });

      setPropertyValidator(WanderLustProperty, [&](PropertyValue v) -> int {

         if (PROPERTY_VALID != isPropertyValueDouble(v)) {
            return PROPERTY_INVALID_TYPE;
         }

         if (std::get<double>(v) < 0.0 || std::get<double>(v) > 1.0) {
            return WANDER_LUST_INVALID_PROBABILITY;
         }

         return PROPERTY_VALID;
      });

      setPropertyValidator(AllegianceProperty, [&](PropertyValue v) -> int {

         if (PROPERTY_VALID != isPropertyValueInt(v)) {
            return PROPERTY_INVALID_TYPE;
         }

         switch (std::get<int>(v)) {

            case FRIEND:
            case ENEMY:
            case NEUTRAL:
               return PROPERTY_VALID;

            default:
               return PROPERTY_INVALID_TYPE;
         }
      });
   }

   /**************************************************************************/

   Creature::Creature(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Being(g, n, std::move(o), std::move(e)) {

      types.push_back(ENTITY_CREATURE);
      setClass("creature");

      setProperty(CounterAttackProperty, DEFAULT_COUNTER_ATTACK);
      setProperty(AllegianceProperty, DEFAULT_ALLEGIANCE);

      setProperty(AutoAttackEnabledProperty, DEFAULT_AUTO_ATTACK_ENABLED);
      setProperty(AutoAttackRepeatProperty, DEFAULT_AUTO_ATTACK_REPEAT);
      setProperty(AutoAttackIntervalProperty, DEFAULT_AUTO_ATTACK_INTERVAL);

      setProperty(WanderEnabledProperty, DEFAULT_WANDER_ENABLED);
      setProperty(WanderIntervalProperty, DEFAULT_WANDER_INTERVAL);
      setProperty(WanderLustProperty, DEFAULT_WANDER_LUST);

      setPropertyValidators();
   }

   /***************************************************************************/

   Creature::Creature(const Creature &c, std::string n): Being(c, n) {}

   /***************************************************************************/

   Creature::Creature(Game *g, const serial::Serializable &data): Being(g, data) {

      setPropertyValidators();
      types.push_back(ENTITY_CREATURE);
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Creature::serialize() {

      std::shared_ptr<serial::Serializable> data = Being::serialize();
      return data;
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

            updateObjectTag = std::make_shared<EntityCallback>([&](std::any data) -> bool {

               auto args = std::any_cast<std::tuple<std::string, Object *>>(data);

               std::string tag = std::get<0>(args);
               Object *object = static_cast<Object *>(std::get<1>(args));

               if (0 == tag.compare(Object::WeaponTag)) {

                  // The weapon tag was added
                  if (object->isTagSet(Object::WeaponTag)) {
                     mutex.lock();
                     weaponCache.insert(object);
                     mutex.unlock();
                  }

                  // The weapon tag was removed
                  else {
                     mutex.lock();
                     weaponCache.erase(object);
                     mutex.unlock();
                  }
               }

               return false;
            });
         }

         if (object->isTagSet(Object::WeaponTag)) {

            mutex.lock();
            weaponCache.insert(object.get());
            mutex.unlock();

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
         mutex.lock();
         weaponCache.erase(object.get());
         mutex.unlock();
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

   /***************************************************************************/

   void Creature::wander(bool overrideEnable) {

      static std::random_device rd;
      static std::mt19937 generator(rd());
      static std::uniform_real_distribution<double> probabilityDist(0, 1);

      // Dead creature's shouldn't be wandering around ;)
      if (!isAlive()) {
         return;
      }

      // make sure wandering isn't turned off
      else if (!overrideEnable && !getProperty<bool>(WanderEnabledProperty)) {
         return;
      }

      else if (auto location = getLocation().lock()) {

         // make sure Creature isn't in some special non-Room Place with no
         // connections
         if (ENTITY_ROOM != location->getType()) {
            return;
         }

         // creature considers moving or staying; which will he pick?
         else if (probabilityDist(generator) > getProperty<double>(WanderLustProperty)) {
            return;
         }

         auto curLoc = std::static_pointer_cast<Room>(location);
         size_t nConnections = curLoc->getNumConnections();

         // don't do anything if Creature is stuck in a room with no exits
         if (!nConnections) {
            return;
         }

         // TODO: use AI to select destination more intelligently; currently random
         // The whole rest of this function will change when we do...
         std::uniform_int_distribution<int> connectionsDist(0, nConnections - 1);

         std::shared_ptr<Room> connection = curLoc->getConnectionByIndex(
            connectionsDist(generator)
         )->getShared();

         // Only proceed if the connection is to a Room that still exists (valid
         // pointer)
         if (connection) {
            gotoLocation(connection);
         }
      }
   }
}
