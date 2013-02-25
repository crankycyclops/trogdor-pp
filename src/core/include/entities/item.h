#ifndef ITEM_H
#define ITEM_H


#include "thing.h"
#include "being.h"


namespace core { namespace entity {

   class Item: public Thing {

      public:

         static const int DEFAULT_WEIGHT = 0;

         static const bool DEFAULT_IS_WEAPON = false;
         static const int DEFAULT_DAMAGE = 0;

         // by default, a Being can take and drop the Item
         static const bool DEFAULT_TAKEABLE = true;
         static const bool DEFAULT_DROPPABLE = true;

      protected:

         Being *owner;     // a Being might own the object
         Place *location;  // the object may be placed in a location

         bool takeable;    // whether or not a Being can take the Item
         bool droppable;   // whether or not a Being can drop the Item
         bool weapon;      // whether or not Item is a weapon

         int weight;       // how much weight Item uses in a Being's inventory
         int damage;       // how much damage Item does if it's a weapon

      public:

         /*
            Constructor for creating a new Item.  Requires reference to the
            containing Game object and a name.
         */
         inline Item(Game *g, string n): Thing(g, n) {

            owner = 0;
            location = 0;
            weight = DEFAULT_WEIGHT;
            takeable = DEFAULT_TAKEABLE;
            droppable = DEFAULT_DROPPABLE;
            weapon = DEFAULT_IS_WEAPON;
            damage = DEFAULT_DAMAGE;
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
            Returns whether or not item is a weapon.

            Input:
               (none)

            Output:
               Bool
         */
         inline bool isWeapon() {return weapon;}

         /*
            Returns damage Item does if it's a weapon, measured in hit points.

            Input:
               (none)

            Output:
               Damage in hit points (int)
         */
         inline int getDamage() {return damage;}

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

         /*
            Sets whether or not Item is a weapon.

            Input:
               Bool

            Output:
               (none)
         */
         inline void setIsWeapon(bool w) {weapon = w;}

         /*
            Sets amount of damage Item does if it's a weapon (measured in hit
            points.)

            Input:
               Damage in hit points (int)

            Output:
               (none)
         */
         inline void setDamage(int d) {damage = d;}
   };
}}


#endif

