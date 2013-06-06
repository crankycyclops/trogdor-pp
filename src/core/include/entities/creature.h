#ifndef CREATURE_H
#define CREATURE_H


#include "being.h"


namespace core { namespace entity {

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
         inline Creature(Game *g, Trogout *o, Trogin *i, string n): Being(g, o, i, n) {

            type = ENTITY_CREATURE;

            allegiance = DEFAULT_ALLEGIANCE;
            counterAttack = DEFAULT_COUNTER_ATTACK;

            autoAttack.enabled = DEFAULT_AUTO_ATTACK_ENABLED;
            autoAttack.interval = DEFAULT_AUTO_ATTACK_INTERVAL;
            autoAttack.repeat = DEFAULT_AUTO_ATTACK_REPEAT;
         }

         /*
            Returns a LuaTable object representing the Creature.  Note that each
            child class should get the value of the parent class's version of
            this method and then fill that object in further with its own
            values.

            Input:
               (none)

            Output:
               LuaTable object
         */
         virtual LuaTable *getLuaTable() const;

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
   };
}}


#endif

