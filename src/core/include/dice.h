#ifndef DICE_H
#define DICE_H


#include <cstdlib>

namespace core {


   /*
      All dice values are doubles between 0 and 1.
   */
   class Dice {

      private:

         double value;

      public:

         /*
            Constructor for the Dice object.
         */
         inline Dice() {roll();}

         /*
            Rolls the dice and returns its new value.

            Input:
               (none)

            Output:
               Dice's new value (double)
         */
         inline double roll() {

            value = (double)rand() / RAND_MAX;
            return value;
         }

         /*
            Return's the dice's current value without changing it.

            Input:
               (none)

            Output:
               Dice's current value (double)
         */
         inline double get() {return value;}
   };
}


#endif
