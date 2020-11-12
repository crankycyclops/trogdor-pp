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

         // by default, a creature will automatically attack when attacked
         static constexpr bool DEFAULT_COUNTER_ATTACK = true;
         static constexpr enum AllegianceType DEFAULT_ALLEGIANCE = NEUTRAL;

         // default auto-attack settings
         static constexpr bool DEFAULT_AUTO_ATTACK_ENABLED = false;
         static constexpr bool DEFAULT_AUTO_ATTACK_REPEAT = false;
         static constexpr int  DEFAULT_AUTO_ATTACK_INTERVAL = 5;

         // Default wander settings
         static constexpr bool DEFAULT_WANDER_ENABLE = false;
         static constexpr int DEFAULT_WANDER_INTERVAL = 10;
         static constexpr double DEFAULT_WANDER_LUST = 0.5;

      private:

         // Compares two Objects according to how much damage they inflict as
         // weapons. Used to keep the Creature's weapon cache sorted.
         struct DamageComparator {

            inline bool operator() (const Object * const &lhs, const Object * const &rhs) const {
               return lhs->getDamage() < lhs->getDamage();
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
         std::shared_ptr<std::function<void(std::any)>> updateObjectTag;

      protected:

         struct {
            bool enabled;
            int  interval;
            bool repeat;
         } autoAttack;

         struct {
            bool   enabled;
            int    interval;
            double wanderlust;
         } wanderSettings;

         // whether creature will fight back when attacked
         bool counterAttack;

         // whether creature is friendly, neutral or aggressive toward others
         enum AllegianceType allegiance;

      public:

         /*
            Constructor for creating a new Creature.  Requires reference to the
            containing Game object and a name.
         */
         Creature(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            Constructor that clones a Creature into another separate and unique
            Creature with identical properties.  Requires a unique name, which
            won't be copied.
         */
         Creature(const Creature &c, std::string n);

         /*
            This constructor deserializes a Creature.

            Input:
               Raw deserialized data (const Serializable &)
         */
         Creature(const serial::Serializable &data);

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
               An object containing easily serializable data (Serializable)
         */
         virtual serial::Serializable serialize();

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
            Returns the Creature's allegiance.

            Input:
               (none)

            Output:
               enum AllegianceType
         */
         inline enum AllegianceType getAllegiance() const {return allegiance;}

         /*
            Returns a string representation of the Creature's allegiance.

            Input:
               (none)

            Output:
               std::string
         */
         inline std::string getAllegianceStr() const {

            switch (allegiance) {

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
            Returns whether or not counterAttack is enabled.

            Input:
               (none)

            Output:
               bool
         */
         inline bool getCounterAttack() {return counterAttack;}

         /*
            Return auto attack parameters.

            Input:
               (none)

            Output:
               enabled:  bool
               repeat:   bool
               interval: int
         */
         inline bool getAutoAttackEnabled()  const {return autoAttack.enabled;}
         inline bool getAutoAttackRepeat()   const {return autoAttack.repeat;}
         inline int  getAutoAttackInterval() const {return autoAttack.interval;}

         /*
            Returns Creature wander settings.

            Input:
               (none)

            Output:
               enabled:    bool
               interval:   int
               wanderlust: double
         */
         inline bool   getWanderEnabled()  const {return wanderSettings.enabled;}
         inline int    getWanderInterval() const {return wanderSettings.interval;}
         inline double getWanderLust()     const {return wanderSettings.wanderlust;}

         /*
            Sets a Creature's allegiance.

            Input:
               AllegianceType

            Output:
               (none)
         */
         inline void setAllegiance(enum AllegianceType a) {

            mutex.lock();
            allegiance = a;
            mutex.unlock();
         }

         /*
            Sets whether or not Creature should automatically fight back when
            attacked.

            Input:
               bool

            Output:
               (none)
         */
         inline void setCounterAttack(bool b) {

            mutex.lock();
            counterAttack = b;
            mutex.unlock();
         }

         /*
            Sets whether or not auto-attack is enabled.

            Input:
               bool

            Output:
               (none)
         */
         inline void setAutoAttackEnabled(bool b) {

            mutex.lock();
            autoAttack.enabled = b;
            mutex.unlock();
         }

         /*
            Sets auto-attack interval.

            Input:
               int

            Output:
               (none)
         */
         inline void setAutoAttackInterval(int i) {

            mutex.lock();
            autoAttack.interval = i;
            mutex.unlock();
         }

         /*
            Sets whether or not auto-attack should continue indefinitely (until
            one of the Beings dies, or until one or both leave the Place.)

            Input:
               bool

            Output:
               (none)
         */
         inline void setAutoAttackRepeat(bool b) {

            mutex.lock();
            autoAttack.repeat = b;
            mutex.unlock();
         }

         /*
            Sets whether or not automatic wandering is enabled.

            Input:
               bool

            Output:
               (none)
         */
         inline void setWanderEnabled(bool b) {

            mutex.lock();
            wanderSettings.enabled = b;
            mutex.unlock();
         }

         /*
            Sets how often (in clock ticks) the Creature considers wandering to
            another location.

            Input:
               int

            Output:
               (none)

            Throws exception if input is invalid (less than 1.)
         */
         inline void setWanderInterval(int i) {

            if (i < 1) {
               throw ValidationException(
                  "Wander interval must be greater than or equal to 1"
               );
            }

            mutex.lock();
            wanderSettings.interval = i;
            mutex.unlock();
         }

         /*
            Sets probability that Creature will wander on each interval.

            Input:
               double

            Output:
               (none)

            Throws exception if input is invalid (not a valid probability.)
         */
         inline void setWanderLust(double d) {

            if (d < 0.0 || d  > 1.0) {
               throw ValidationException("Probability must be between 0 and 1");
            }

            mutex.lock();
            wanderSettings.wanderlust = d;
            mutex.unlock();
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
