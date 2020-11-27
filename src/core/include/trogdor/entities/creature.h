#ifndef CREATURE_H
#define CREATURE_H


#include <memory>

#include <trogdor/entities/being.h>
#include <trogdor/exception/validationexception.h>


namespace trogdor::entity {


   class Creature: public Being {

      public:

         enum AllegianceType {
            FRIEND,
            NEUTRAL,
            ENEMY
         };

         // Wander interval property must be greater than or equal to 1
         static constexpr int WANDER_INTERVAL_LESS_THAN_ONE = 2;

         // Wander lust must be a valid probability (between 0 and 1)
         static constexpr int WANDER_LUST_INVALID_PROBABILITY = 3;

         // Whether creature is friendly, neutral or aggressive toward others
         static constexpr const char *AllegianceProperty = "allegiance";

         // Whether or not the Creature will fight back when attacked
         static constexpr const char *CounterAttackProperty = "counterAttack";

         // Whether or not the creature should automatically attack Players and
         // other Creatures when they enter the same room
         static constexpr const char *AutoAttackEnabledProperty = "autoattack.enabled";

         // If set to true, the Creature will repeat its attack every n number
         // of clock ticks, where n is determined by AutoAttackIntervalProperty
         static constexpr const char *AutoAttackRepeatProperty = "autoattack.repeat";

         // If autoattack is enabled, this is the number of click ticks that
         // will pass before the Creature attacks again (if repeat is set to true)
         static constexpr const char *AutoAttackIntervalProperty = "autoattack.interval";

         // If set to true, the Creature will wander about through different rooms
         static constexpr const char *WanderEnabledProperty = "wander.enabled";

         // If wandering is enabled, this is the number of clock ticks that will
         // pass before the Creature considers moving to another room
         static constexpr const char *WanderIntervalProperty = "wander.interval";

         // This represents the probability that the Creature will actually move
         // each time it considers doing so
         static constexpr const char *WanderLustProperty = "wander.lust";

         // by default, a creature will automatically attack when attacked
         static constexpr bool DEFAULT_COUNTER_ATTACK = true;
         static constexpr enum AllegianceType DEFAULT_ALLEGIANCE = NEUTRAL;

         // default auto-attack settings
         static constexpr bool DEFAULT_AUTO_ATTACK_ENABLED = false;
         static constexpr bool DEFAULT_AUTO_ATTACK_REPEAT = false;
         static constexpr int  DEFAULT_AUTO_ATTACK_INTERVAL = 5;

         // Default wander settings
         static constexpr bool DEFAULT_WANDER_ENABLED = false;
         static constexpr int DEFAULT_WANDER_INTERVAL = 10;
         static constexpr double DEFAULT_WANDER_LUST = 0.5;

      private:

         // Compares two Objects according to how much damage they inflict as
         // weapons. Used to keep the Creature's weapon cache sorted.
         struct DamageComparator {

            inline bool operator() (const Object * const &lhs, const Object * const &rhs) const {
               return lhs->getProperty<int>(Object::DamageProperty) < rhs->getProperty<int>(Object::DamageProperty);
            }
         };

         // Cached set of weapons in the Creature's inventory, sorted from
         // greatest to least amount of damage done. Used to speed up automatic
         // weapon selection during combat. I'm going to leave this as a set of
         // raw pointers instead of instances of std::weak_ptr because I know
         // that any Object that's in this set is still owned by the Creature's
         // inventory and therefore is still good.
         std::set<Object *, DamageComparator> weaponCache;

         // This callback, which fires whenever a tag is added to or removed
         // from an inventory object, will add the object to the Creature's
         // weapon cache if it's a weapon and remove it if it's not. This
         // callback is added by insertIntoInventory() and removed by
         // removeFromInventory(). This doesn't actually get initialized until
         // the first time an object is added to a creature's inventory.
         std::shared_ptr<std::function<bool(std::any)>> updateObjectTag;

         /*
            Sets property validators for all properties settable by Creature.

            Input:
               (none)

            Output:
               (none)
         */
         void setPropertyValidators();

      public:

         /*
            Constructor for creating a new Creature.  Requires reference to the
            containing Game object and a name.
         */
         Creature(
            Game *g,
            std::string n,
            std::unique_ptr<Trogout> o,
            std::unique_ptr<Trogerr> e
         );

         /*
            Constructor that clones a Creature into another separate and unique
            Creature with identical properties.  Requires a unique name, which
            won't be copied.
         */
         Creature(const Creature &c, std::string n);

         /*
            This constructor deserializes a Creature.

            Input:
               Game the Creature belongs to (Game *)
               Raw deserialized data (const Serializable &)
               Pointer to output stream object (std::unique_ptr<Trogout>)
               Pointer to error stream object (std::unique_ptr<Trogerr>)
         */
         Creature(
            Game *g,
            const serial::Serializable &data,
            std::unique_ptr<Trogout> o,
            std::unique_ptr<Trogerr> e
         );

         /*
            Returns a smart pointer representing a raw Creature pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Creature>
         */
         inline std::shared_ptr<Creature> getShared() {

            return std::dynamic_pointer_cast<Creature>(Entity::getShared());
         }

         /*
            Serializes the Creature.

            Input:
               (none)

            Output:
               An object containing easily serializable data (std::shared_ptr<Serializable>)
         */
         virtual std::shared_ptr<serial::Serializable> serialize();

         /*
            Calls Being::insertIntoInventory and then, if necessary, updates the
            Creature's weapon cache.

            Input:
               See Being::insertIntoInventory for documentation

            Output:
               bool (true on success and false on failure)
         */
         virtual bool insertIntoInventory(
            const std::shared_ptr<Object> &object,
            bool considerWeight = true
         );

         /*
            Calls Being::removeFromInventory and then, if necessary, updates the
            Creature's weapon cache.

            Input:
               See Being::removeFromInventory for documentation

            Output:
               (none)
         */
         virtual void removeFromInventory(const std::shared_ptr<Object> &object);

         /*
            Returns a string representation of the Creature's allegiance.

            Input:
               (none)

            Output:
               std::string
         */
         inline std::string getAllegianceStr() const {

            switch (getProperty<int>(AllegianceProperty)) {

               case FRIEND:
                  return "friend";

               case NEUTRAL:
                  return "neutral";

               case ENEMY:
                  return "enemy";
            }

            return "undefined";
         }

         /*
            Selects a weapon from the Creature's inventory, if one is available.
            If no weapon is available, return 0.

            This function will use the Creature's dexterity to determine how
            likely it is that the Creature will be able to grab a weapon from
            its inventory for an attack instead of using its bare hands.

            Input:
               (none)

            Output:
               Object* (weapon, or 0 if none)
         */
         Object *selectWeapon();

         /*
            Triggers the Creature to possibly wander into another room subject
            to the values set in wanderSettings. If the Creature is dead, they
            will not move, since wandering dead creatures doesn't make sense.

            Input:
               Whether or not to allow wandering even if it's ordinarily
               disabled (default: false)

            Output:
               (none)
         */
         void wander(bool overrideEnable = false);
   };
}


#endif
