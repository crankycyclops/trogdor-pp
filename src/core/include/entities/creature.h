#ifndef CREATURE_H
#define CREATURE_H


#include "../dice.h"
#include "being.h"

#include "../iostream/nullin.h"


namespace trogdor { namespace core { namespace entity {

   class Creature: public Being {

      public:

         enum AllegianceType {
            FRIEND,
            NEUTRAL,
            ENEMY
         };

         // by default, a creature will automatically attack when attacked
         static const bool DEFAULT_COUNTER_ATTACK = true;
         static const enum AllegianceType DEFAULT_ALLEGIANCE = NEUTRAL;

         // default auto-attack settings
         static const bool DEFAULT_AUTO_ATTACK_ENABLED = false;
         static const bool DEFAULT_AUTO_ATTACK_REPEAT = false;
         static const int  DEFAULT_AUTO_ATTACK_INTERVAL = 5;

      private:

         // Cached list of weapons in the Creature's inventory, sorted from
         // greatest to least amount of damage done. Used to speed up
         // intelligent weapon selection during combat.
         ObjectList weaponCache;

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
         inline Creature(Game *g, string n, Trogout *o, Trogout *e):
         Being(g, n, o, new NullIn(), e),
         allegiance(DEFAULT_ALLEGIANCE), counterAttack(DEFAULT_COUNTER_ATTACK) {

            types.push_back(ENTITY_CREATURE);
            setClass("creature");

            autoAttack.enabled = DEFAULT_AUTO_ATTACK_ENABLED;
            autoAttack.interval = DEFAULT_AUTO_ATTACK_INTERVAL;
            autoAttack.repeat = DEFAULT_AUTO_ATTACK_REPEAT;
         }

         /*
            Constructor that clones a Creature into another separate and unique
            Creature with identical properties.  Requires a unique name, which
            won't be copied.
         */
         inline Creature(const Creature &c, string n): Being(c, n) {

            autoAttack = c.autoAttack;
            wanderSettings = c.wanderSettings;
            counterAttack = c.counterAttack;
            allegiance = c.allegiance;
         }

         /*
            Clears the cached sorted list of weapons in the Creature's inventory.
            The interface needs to be public so that if an Object in the
            Creature's inventory has its weapon attribute turned off, perhaps by
            a script, then Object::setWeapon() can signal the Creature that its
            cache needs to be rebuilt.

            Input:
               (none)

            Output:
               (none)
         */
         inline void clearWeaponCache() {weaponCache.clear();}

         /*
            Builds cached sorted list of weapons in the Creature's inventory.
            This is used to speed up weapon selection during combat, so we don't
            have to constantly build this list on the fly.

            Input:
               (none)

            Output:
               (none)
         */
         void buildWeaponCache();

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
         inline string getAllegianceStr() const {

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
         inline void setAllegiance(enum AllegianceType a) {allegiance = a;}

         /*
            Sets whether or not Creature should automatically fight back when
            attacked.

            Input:
               bool

            Output:
               (none)
         */
         inline void setCounterAttack(bool b) {counterAttack = b;}

         /*
            Sets whether or not auto-attack is enabled.

            Input:
               bool

            Output:
               (none)
         */
         inline void setAutoAttackEnabled(bool b) {autoAttack.enabled = b;}

         /*
            Sets auto-attack interval.

            Input:
               int

            Output:
               (none)
         */
         inline void setAutoAttackInterval(int i) {autoAttack.interval = i;}

         /*
            Sets whether or not auto-attack should continue indefinitely (until
            one of the Beings dies, or until one or both leave the Place.)

            Input:
               bool

            Output:
               (none)
         */
         inline void setAutoAttackRepeat(bool b) {autoAttack.repeat = b;}

         /*
            Sets whether or not automatic wandering is enabled.

            Input:
               bool

            Output:
               (none)
         */
         inline void setWanderEnabled(bool b) {wanderSettings.enabled = b;}

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
               throw "Wander interval must be greater than or equal to 1";
            }

            wanderSettings.interval = i;
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
               throw "Probability must be between 0 and 1";
            }

            wanderSettings.wanderlust = d;
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
   };
}}}


#endif

