#ifndef BEING_H
#define BEING_H


#include "thing.h"


namespace core { namespace entity {

   class Being: public Thing {

      private:

         // TODO: attributes class
         // TODO: inventory class

         int health;    // number of health points the being currently has
         int maxHealth; // maximum number of health points (0 for immortal)
         bool alive;    // whether or not the being is alive

         bool attackable;  // whether or not being can be attacked
         double woundRate; // max probability of being hit when attacked

      public:
   };
}}


#endif

