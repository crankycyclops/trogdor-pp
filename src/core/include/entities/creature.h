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
