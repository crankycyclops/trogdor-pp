#ifndef CREATURE_H
#define CREATURE_H


#include "being.h"


namespace core { namespace entity {

   class Creature: public Being {

      protected:

         int  allegiance;    // whether creature is friendly, neutral or aggressive
         bool counterAttack; // whether or not creature will fight back when attacked

      public:

         /*
            Constructor for creating a new Creature.  Requires reference to the
            containing Game object and a name.
         */
         inline Creature(Game *g, string n): Being(g, n) {}
   };
}}


#endif

