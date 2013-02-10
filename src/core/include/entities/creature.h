#ifndef CREATURE_H
#define CREATURE_H


#include "being.h"


namespace core { namespace entity {

   class Creature: public Being {

      private:

         int  allegiance;    // whether creature is friendly, neutral or aggressive
         bool counterAttack; // whether or not creature will fight back when attacked

      public:
   };
}}


#endif

