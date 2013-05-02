#ifndef BEING_H
#define BEING_H


#include <set>
#include <boost/unordered_map.hpp>

#include "../utility.h"
#include "../dice.h"

#include "thing.h"
#include "place.h"

using namespace boost;


namespace core { namespace entity {

   class Being: public Thing {

      public:

         static const int DEFAULT_ATTRIBUTE_STRENGTH     = 10;
         static const int DEFAULT_ATTRIBUTE_DEXTERITY    = 10;
         static const int DEFAULT_ATTRIBUTE_INTELLIGENCE = 10;

         static const int DEFAULT_INVENTORY_WEIGHT = 0;

         static const bool DEFAULT_ATTACKABLE = true;
         static const int DEFAULT_DAMAGE_BARE_HANDS = 5;

         enum takeError {
            TAKE_TOO_HEAVY,
            TAKE_UNTAKEABLE
         };

         enum dropError {
            DROP_UNDROPPABLE
         };

      protected:

         int health;            // number of health points the being currently has
         int maxHealth;         // maximum number of health points (0 for immortal)
         bool alive;            // whether or not the being is alive

         bool attackable;       // whether or not being can be attacked
         double woundRate;      // max probability of being hit when attacked
         int damageBareHands;   // damage done during combat without a weapon

         struct {
            int strength;
            int dexterity;
            int intelligence;
            int initialTotal;   // total attributes that the Being started with
         } attributes;

         struct {
            int weight;         // how much weight inventory can hold
            int currentWeight;  // how much weight is currently used
            unsigned count;     // number of items in the inventory
            ObjectSet objects;
            ObjectsByNameMap objectsByName;
         } inventory;

         /*
            Calculates whether or not an attack will be successful.

            Input:
               defender (Being)

            Output:
               true if successful and false if not
         */
         inline bool isAttackSuccessful(Being *defender) {

            Dice d;

            // probability that the attack will be successful
            double p = CLAMP(getStrengthFactor() * (defender->woundRate / 2) +
               (defender->woundRate / 2), 0.0, defender->woundRate);

            if (d.get() < p) {
               return true;
            }

            else {
               return false;
            }
         }

      public:

         /*
            Constructor for creating a new Being.  Requires reference to the
            containing Game object and a name.
         */
         inline Being(Game *g, string n): Thing(g, n) {

            attackable = DEFAULT_ATTACKABLE;
            damageBareHands = DEFAULT_DAMAGE_BARE_HANDS;

            attributes.strength = DEFAULT_ATTRIBUTE_STRENGTH;
            attributes.dexterity = DEFAULT_ATTRIBUTE_DEXTERITY;
            attributes.intelligence = DEFAULT_ATTRIBUTE_INTELLIGENCE;
            attributes.initialTotal = DEFAULT_ATTRIBUTE_STRENGTH +
               DEFAULT_ATTRIBUTE_DEXTERITY + DEFAULT_ATTRIBUTE_INTELLIGENCE;

            inventory.count = 0;
            inventory.weight = DEFAULT_INVENTORY_WEIGHT;
            inventory.currentWeight = 0;
         }

         /*
            Returns pointer to a list of all Objects that match the given name
            in the Being's inventory, or a NULL pointer if there are no matches.

            Input:
               name (string)

            Output:
               ObjectList * (NULL if no matches)
         */
         inline ObjectList *getInventoryObjectsByName(string name) {

            ObjectsByNameMap::iterator i = inventory.objectsByName.find(name);

            if (i == inventory.objectsByName.end()) {
               return 0;
            }

            else {
               return &(i->second);
            }
         }

         /*
            Returns whether or not the Being is alive.

            Input:
               (none)

            Output:
               bool
         */
         inline bool isAlive() {return alive;}

         /*
            Returns the maximum weight of the Being's inventory.

            Input:
               (none)

            Output:
               max weight (int)
         */
         inline int const getInventoryMaxWeight() const {return inventory.weight;}

         /*
            Returns the current weight of the Being's inventory.

            Input:
               (none)

            Output:
               current weight (int)
         */
         inline int const getInventoryCurWeight() const {return inventory.currentWeight;}

         /*
            Returns the number of items in the Being's inventory.

            Input:
               (none)

            Output:
               Number of items (unsigned int)
         */
         inline unsigned const getInventoryCount() const {return inventory.count;}

         /*
            Returns an iterator which will iterate through all items in the
            Being's inventory.

            Input:
               (none)

            Output:
               ObjectSet::const_iterator
         */
         inline ObjectSet::iterator getInventoryIterator() const {

            return inventory.objects.begin();
         }

         /*
            Returns whether or not we've iterated past the end of our set of
            items in the Player's inventory.

            Input:
               Current iterator

            Output:
               true if we've reached the end and false if not
         */
         inline bool isInventoryEnd(ObjectSet::iterator i) const {

            return (i == inventory.objects.end());
         }

         /*
            Return raw point values for each attribute.

            Input:
               (none)

            Output:
               Attribute value in points (int)
         */
         inline int getStrength() {return attributes.strength;}
         inline int getDexterity() {return attributes.dexterity;}
         inline int getIntelligence() {return attributes.intelligence;}

         /*
            Return the relative factors of each attribute.

            Input:
               (none)

            Ouput:
               Attribute factor (double)
         */
         inline double getStrengthFactor() {

            return (double)attributes.strength / (double)attributes.initialTotal;
         }

         inline double getDexterityFactor() {

            return (double)attributes.dexterity / (double)attributes.initialTotal;
         }

         inline double getIntelligenceFactor() {

            return (double)attributes.intelligence / (double)attributes.initialTotal;
         }

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

         /*
            Sets the Being's attributes.

            Input:
               Attribute value (int)

            Output:
               (none)
         */
         inline void setStrength(int s) {attributes.strength = s;}
         inline void setDexterity(int d) {attributes.dexterity = d;}
         inline void setIntelligence(int i) {attributes.intelligence = i;}

         /*
            This should only be called when the Being is first initialized, and
            sets the value of attributes.initialTotal to the sum of all the
            currently set attributes.  This value is used to calculate factors
            for each attribute, which are relative to the Being's total initial
            number of attributes.

            Input:
               (none)

            Output:
               (none)
         */
         inline void setAttributesInitialTotal() {

            attributes.initialTotal = attributes.strength + attributes.dexterity
               + attributes.intelligence;
         }

         /*
            Sets the inventory's weight.  0 means the Being  can carry an
            unlimited number of items.

            Input:
               Weight (int)

            Output:
               (none)
         */
         inline void setInventoryWeight(int w) {inventory.weight = w;}

         /*
            Inserts the given object into the Being's inventory.  Returns true
            if the Object fits (there's enough free weight.)  Otherwise, nothing
            happens and false is returned.  By default, we check the inventory's
            available weight before inserting, but passing false to the second
            argument will bypass this check (should only be used during the
            parsing of game.xml.)

            Input:
               Object *
               Whether or not to check the inventory's weight before inserting (default = true)

            Output:
               bool (true on success and false on failure)
         */
         bool insertIntoInventory(Object *object, bool considerWeight = true);

         /*
            Removes an object from a Being's inventory.  If the Object isn't in
            the Being's inventory, then nothing happens.

            Input:
               Pointer to Object to remove

            Output:
               (none)
         */
         void removeFromInventory(Object *object);

         /*
            Make the Being move to the specified Place.  This both sets the
            Being's location and has the Being observe the Place.  Events are
            triggered for both actions.

            Input:
               Place where Being should go

            Output:
               (none)
         */
         void gotoLocation(Place *location);

         /*
            Allows a Being to take an object.  Calls insertIntoInventory() and
            triggers the appropriate events.

            Input:
               Object to take

            Output:
               (none)

            Exceptions:
               Throws the following errors:
                  TAKE_TOO_HEAVY - object is too heavy
                  TAKE_UNTAKEABLE - attempted to take an untakeable object
         */
         void take(Object *object);

         /*
            Allows a Being to drop an object.  Calls removeIntoInventory() and
            triggers the appropriate events.

            Input:
               Object to drop

            Output:
               (none)

            Exceptions:
               Throws the following errors:
                  DROP_UNDROPPABLE - attempt to drop an undroppable object
         */
         void drop(Object *object);

         /*
            Adds health to the Being.

            Input:
               How many health points to add (int)
               Whether or not to allow health to exceed maxHealth (bool)

            Output:
               (none)
         */
         void addHealth(int up, bool allowOverflow = false);

         /*
            Removes health from the Being.

            Input:
               How many health points to remove (int)
               Whether or not to let the Being die if health is <= 0 (bool)

            Output:
               (none)
         */
         void removeHealth(int down, bool allowDeath = true);

         /*
            Make the Being die.  Triggers an event.

            Input:
               (none)

            Output:
               (none)
         */
         void die();
   };
}}


#endif

