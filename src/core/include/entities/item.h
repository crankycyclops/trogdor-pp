#ifndef ITEM_H
#define ITEM_H


#include "thing.h"
#include "being.h"


namespace core { namespace entity {

   class Item: public Thing {

      public:

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

         int damage;       // how much damage Item does if it's a weapon

      public:

         /*
            Constructor for creating a new Item.  Requires reference to the
            containing Game object and a name.
         */
         inline Item(Game *g, string n): Thing(g, n) {

            owner = 0;
            location = 0;
            takeable = DEFAULT_TAKEABLE;
            droppable = DEFAULT_DROPPABLE;
            weapon = DEFAULT_IS_WEAPON;
            damage = DEFAULT_DAMAGE;
         }

         /*
            Returns the owner of the object (0 if there is no owner.)

            Input:
               (none)

            Output:
               Being *
         */
         inline Being *getOwner() const {return owner;}

         /*
            Returns the object's current location (0 if it isn't placed
            anywhere.)

            Input:
               (none)

            Output:
               Place *
         */
         inline Place *getLocation() const {return location;}

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
            Sets the owner.

            Input:
               Being that should now own the object

            Output:
               (none)
         */
         inline void setOwner(Being *being) {owner = being;}

         /*
            Sets the Object's location.

            Input:
               Place that should now contain the object

            Output:
               (none)
         */
         inline void setLocation(Place *place) {location = place;}

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

