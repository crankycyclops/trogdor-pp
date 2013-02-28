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

      protected:

         bool counterAttack; // whether creature will fight back when attacked

         // whether creature is friendly, neutral or aggressive toward others
         enum AllegianceType allegiance;

      public:

         /*
            Constructor for creating a new Creature.  Requires reference to the
            containing Game object and a name.
         */
         inline Creature(Game *g, string n): Being(g, n) {

            type = ENTITY_CREATURE;

            allegiance = DEFAULT_ALLEGIANCE;
            counterAttack = DEFAULT_COUNTER_ATTACK;
         }

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
   };
}}


#endif
