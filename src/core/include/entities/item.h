#ifndef ITEM_H
#define ITEM_H


#include "thing.h"
#include "being.h"


namespace core { namespace entity {

   class Item: public Thing {

      public:

         static const int DEFAULT_WEIGHT = 0;

         // by default, a Being can take and drop the Item
         static const bool DEFAULT_TAKEABLE = true;
         static const bool DEFAULT_DROPPABLE = true;

      protected:

         Being *owner;

         bool takeable;    // whether or not a Being can take the Item
         bool droppable;   // whether or not a Being can drop the Item

         int weight;       // how much weight Item uses in a Being's inventory

      public:

         /*
            Constructor for creating a new Item.  Requires reference to the
            containing Game object and a name.
         */
         inline Item(Game *g, string n): Thing(g, n) {

            weight = DEFAULT_WEIGHT;
            takeable = DEFAULT_TAKEABLE;
            droppable = DEFAULT_DROPPABLE;
         }

         /*
            Returns the Item's weight.

            Input:
               (none)

            Output:
               The weight (int)
         */
         inline int getWeight() {return weight;}

         /*
            Returns whether or not the Item can be taken.

            Input:
               (none)

            Output:
               bool
         */
         inline bool getTakeable() {return takeable;}

         /*
            Returns whether or not the Item can be dropped.

            Input:
               (none)

            Output:
               bool
         */
         inline bool getDroppable() {return droppable;}

         /*
            Sets the Item's weight (how much space it uses in a Being's
            inventory.)

            Input:
               New weight (int)

            Output:
               (none)
         */
         inline void setWeight(int w) {weight = w;}

         /*
            Sets whether or not a Being can take the object.

            Input:
               bool

            Output:
               (none)
         */
         inline void setTakeable(bool t) {takeable = t;}

         /*
            Sets whether or not a Being can drop the object.

            Input:
               bool

            Output:
               (none)
         */
         inline void setDroppable(bool d) {droppable = d;}
   };
}}


#endif

