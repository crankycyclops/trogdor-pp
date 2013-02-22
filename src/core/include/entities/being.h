#ifndef BEING_H
#define BEING_H


#include "thing.h"


namespace core { namespace entity {

   class Being: public Thing {

      protected:

         // TODO: attributes class
         // TODO: inventory class

         int health;    // number of health points the being currently has
         int maxHealth; // maximum number of health points (0 for immortal)
         bool alive;    // whether or not the being is alive

         bool attackable;  // whether or not being can be attacked
         double woundRate; // max probability of being hit when attacked

      public:

         /*
            Constructor for creating a new Being.  Requires reference to the
            containing Game object and a name.
         */
         inline Being(Game *g, string n): Thing(g, n) {}

         /*
            Puts the Being in either an alive (true) or a dead (false) state.

            Input:
               True for alive and False for dead

            Output:
               (none)
         */
         inline void setAlive(bool state) {alive = state;}

         /*
            Sets the Being's health.

            Input:
               Integer number of health points

            Output:
               (none)
         */
         inline void setHealth(int h) {

            // TODO: should I intelligently handle player dying here?
            health = h;
         }

         /*
            Sets the Being's maximum health (0 for immortal.)

            Input:
               Integer number of health points

            Output:
               (none)
         */
         inline void setMaxHealth(int h) {maxHealth = h;}

         /*
            Sets whether or not Being can be attacked.

            Input:
               Boolean

            Output:
               (none)
         */
         inline void setAttackable(bool a) {attackable = a;}

         /*
            Sets Being's wound rate, which is a factor in how likely it is to be
            hit during an attack.

            Input:
               Double

            Output:
               (none)
         */
         inline void setWoundRate(double rate) {woundRate = rate;}
   };
}}


#endif

