#ifndef BEING_H
#define BEING_H


#include <set>
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "../utility.h"
#include "../dice.h"

#include "thing.h"
#include "place.h"
#include "object.h"

#include "../exception/undefinedexception.h"


namespace trogdor { namespace entity {


   class Being: public Thing {

      // addAlias needs to be able to call indexInventoryItemName()
      friend void Object::addAlias(string alias);

      public:

         static const int DEFAULT_ATTRIBUTE_STRENGTH     = 10;
         static const int DEFAULT_ATTRIBUTE_DEXTERITY    = 10;
         static const int DEFAULT_ATTRIBUTE_INTELLIGENCE = 10;

         static const bool DEFAULT_RESPAWN_ENABLED = false;
         static const int DEFAULT_RESPAWN_INTERVAL = 0;
         static const int DEFAULT_RESPAWN_LIVES = -1;

         static const int DEFAULT_INVENTORY_WEIGHT = 0;

         static const bool DEFAULT_ATTACKABLE = true;
         static const int DEFAULT_DAMAGE_BARE_HANDS = 5;

      protected:

         int health;            // number of health points the being currently has
         int maxHealth;         // maximum number of health points (0 for immortal)
         bool alive;            // whether or not the being is alive

         bool attackable;       // whether or not being can be attacked
         double woundRate;      // max probability of being hit when attacked
         int damageBareHands;   // damage done during combat without a weapon

         struct {
            bool enabled;
            int  interval;
            int  lives;
         } respawnSettings;

         typedef unordered_map<string, int> AttributesMap;

         struct {
            AttributesMap values;
            int initialTotal;   // total attributes that the Being started with
         } attributes;

         struct {
            int weight;         // how much weight inventory can hold
            int currentWeight;  // how much weight is currently used
            unsigned count;     // number of objects in the inventory
            ObjectSet objects;
            ObjectsByNameMap objectsByName;
         } inventory;

         /*
            Overrides Entity::display() in order to handle the description of
            living vs. deceased Beings.

            Input:
               Being observing the Place
               Whether or not to always show the long description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

         /*
            Overrides Entity::displayShort in order to handle the short
            description of living vs. deceased Beings.

            Input:
               Being doing the observing

            Output:
               (none)
         */
         virtual void displayShort(Being *observer);

         /*
            Indexes an inventory Object's alias so that it can be referenced by
            name.

            Input:
               Alias (string)
               Object to be indexed (Object *)

            Output:
               (none)
         */
         inline void indexInventoryItemName(string alias, Object *object) {

            if (inventory.objectsByName.find(alias) == inventory.objectsByName.end()) {
               ObjectList newList;
               inventory.objectsByName[alias] = newList;
            }

            inventory.objectsByName.find(alias)->second.push_back(object);
         }

         /*
            Calculates amount of damage (in hit points) that we do when we
            attack another Being.

            Input:
               defender (Being)

            Output:
               true if successful and false if not
         */
         int calcDamage(Being *defender, Object *weapon = 0);

         /*
            When a Being is attacked, Being::calcDamage() is used to calculate
            how many points to subtract from the defender's health based on
            that Being's attributes and the weapon used.  This ratio, multiplied
            by that value, will determine how many points are actually
            subtracted.  This allows us to take things like armor into account.
            If there is no buffer against the attack, then we will return 1, and
            the defender will suffer all the damage.  If the defender has some
            protection, then this value will be less than 1, and so the total
            number of points subtracted will be less than the initially
            calculated value.

            NOTE: we can also return values > 1 for effects like double damage,
            etc.

            Input:
               (none)

            Output:
               A ratio that determines how much of the damage calculated in a
               successful attack against the Being is actually subtracted from
               the Being's health (double.)
         */
         double getDamageRatio();

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
            double p = CLAMP(getAttributeFactor("strength") * (defender->woundRate / 2) +
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
         inline Being(Game *g, string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogout> e): Thing(g, n,
         std::move(o), std::move(i), std::move(e)), attackable(DEFAULT_ATTACKABLE),
         damageBareHands(DEFAULT_DAMAGE_BARE_HANDS) {

            respawnSettings.enabled  = DEFAULT_RESPAWN_ENABLED;
            respawnSettings.interval = DEFAULT_RESPAWN_INTERVAL;
            respawnSettings.lives    = DEFAULT_RESPAWN_LIVES;

            setAttribute("strength", DEFAULT_ATTRIBUTE_STRENGTH);
            setAttribute("dexterity", DEFAULT_ATTRIBUTE_DEXTERITY);
            setAttribute("intelligence", DEFAULT_ATTRIBUTE_INTELLIGENCE);
            setAttributesInitialTotal();

            inventory.count         = 0;
            inventory.weight        = DEFAULT_INVENTORY_WEIGHT;
            inventory.currentWeight = 0;

            types.push_back(ENTITY_BEING);
         }

         /*
            Constructor that clones a Being into another separate and unique
            Being with identical properties.  Requires a unique name, which
            won't be copied.

            NOTE: inventory objects will NOT be copied, in order to maintain
            sanity -- it doesn't make sense for two Beings to simultaneously
            carry the same object, right? ;)
         */
         inline Being(const Being &b, string n): Thing(b, n) {

            health = b.health;
            maxHealth = b.maxHealth;
            alive = b.alive;
            attackable = b.attackable;
            woundRate = b.woundRate;
            damageBareHands = b.damageBareHands;

            respawnSettings.enabled = b.respawnSettings.enabled;
            respawnSettings.interval = b.respawnSettings.interval;
            respawnSettings.lives = b.respawnSettings.lives;

            attributes.values = b.attributes.values;
            attributes.initialTotal = b.attributes.initialTotal;

            inventory.count = 0;
            inventory.weight = b.inventory.weight;
            inventory.currentWeight = 0;
         }

         /*
            Returns iterators over all Objects that match the given name in the
            Being's inventory.

            Input:
               name (string)

            Output:
               Pair of begin and end iterators (ObjectListCItPair)
         */
         inline ObjectListCItPair getInventoryObjectsByName(string name) const {

            ObjectListCItPair objects;
            ObjectsByNameMap::const_iterator i = inventory.objectsByName.find(name);

            if (i == inventory.objectsByName.end()) {
               objects.begin = emptyObjectList.begin();
               objects.end   = emptyObjectList.end();
            }

            else {
               objects.begin = i->second.begin();
               objects.end   = i->second.end();
            }

            return objects;
         }

         /*
            Returns amount of damage Being does with its bare hands.

            Input:
               (none)

            Output:
               int
         */
         inline int getDamageBareHands() const {return damageBareHands;}

         /*
            Returns whether or not the Being is alive.

            Input:
               (none)

            Output:
               bool
         */
         inline bool isAlive() const {return alive;}

         /*
            Returns true if the Being is immortal and false if it's not.

            Input:
               (none)

            Output:
               bool
         */
         inline bool isImmortal() const {return maxHealth == 0 ? true : false;}

         /*
            Returns true if the Being can be attacked and false if it can't.

            Input:
               (none)

            Output:
               bool
         */
         inline bool isAttackable() const {return attackable;}

         /*
            Indicates whether or not respawning is enabled for this Being.

            Input:
               (none)

            Output:
               true or false (bool)
         */
         inline bool getRespawnEnabled() const {return respawnSettings.enabled;}

         /*
            Returns the number of clock ticks that should pass before Being
            respawns after dying.

            Input:
               (none)

            Output:
               number of clock ticks (int)
         */
         inline int getRespawnInterval() const {return respawnSettings.interval;}

         /*
            Returns the number of times a Being can respawn before dying
            permanently.

            Input:
               (none)

            Output:
               number of lives left; -1 indicates unlimited lives (int)
         */
         inline int getRespawnLives() const {return respawnSettings.lives;}

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
            Returns pair of const_iterators over the Objects in the Being's
            inventory.

            Input:
               (none)

            Output:
               inventory.objects.begin() and end() (ObjectSetCItPair)
         */
         inline ObjectSetCItPair getInventoryObjects() const {

            ObjectSetCItPair objects;

            objects.begin = inventory.objects.begin();
            objects.end   = inventory.objects.end();

            return objects;
         }

         /*
            Return raw point value for an attribute.

            Input:
               Name of the attribute whose value you wish to retrieve

            Output:
               Attribute value in points (int)
         */
         inline int getAttribute(string key) const {

            if (attributes.values.find(key) == attributes.values.end()) {
               throw UndefinedException(string("attribute '") + key + "' not set!");
            }

            return attributes.values.find(key)->second;
         }

         /*
            Return the relative factor of an attribute.

            Input:
               (none)

            Ouput:
               Attribute factor (double)
         */
         inline double getAttributeFactor(string key) const {

            return (double)getAttribute(key) / (double)attributes.initialTotal;
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
            Sets the amount of damage done by the Being's bare hands.

            Input:
               Int

            Output:
               (none)
         */
         inline void setDamageBareHands(int d) {damageBareHands = d;}

         /*
            Setters for respawn settings.

            Input:
               values (bool or int)

            Output:
               (none)
         */
         inline void setRespawnEnabled(bool b) {respawnSettings.enabled = b;}
         inline void setRespawnInterval(int i) {respawnSettings.interval = i;}
         inline void setRespawnLives(int i) {respawnSettings.lives = i;}

         /*
            Increments the Being's number of lives.

            Input:
               (none)

            Output:
               (none)
         */
         inline void incRespawnLives() {

            // only increment number of lives if not unlimited
            if (respawnSettings.lives > -1) {
               respawnSettings.lives++;
            }
         }

         /*
            Sets an attribute.

            Input:
               Name of the attribute (string)
               Attribute value (int)

            Output:
               (none)
         */
         inline void setAttribute(string key, int s) {attributes.values[key] = s;}

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

            attributes.initialTotal = 0;

            for (AttributesMap::iterator i = attributes.values.begin();
            i != attributes.values.end(); i++) {
               attributes.initialTotal += i->second;
            }
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
            the Being's inventory, nothing happens.

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
               Whether or not to check for the taking of "untakeable objects"
               Whether or not to trigger before and after take events

            Output:
               (none)

            Exceptions:
               Throws the following errors:
                  TAKE_TOO_HEAVY - object is too heavy
                  TAKE_UNTAKEABLE - attempted to take an untakeable object
         */
         void take(Object *object, bool checkUntakeable = true, bool doEvents = true);

         /*
            Allows a Being to drop an object.  Calls removeIntoInventory() and
            triggers the appropriate events.

            Input:
               Object to drop
               Whether or not to check for the dropping of "undroppable objects"
               Whether or not to trigger before and after drop events

            Output:
               (none)

            Exceptions:
               Throws the following errors:
                  DROP_UNDROPPABLE - attempt to drop an undroppable object
         */
         void drop(Object *object, bool checkUndroppable = true, bool doEvents = true);

         /*
            Initiate an attack against defender, possibly using an optional
            weapon.

            Input:
               Being to attack (Being *)
               Optional weapon (Object *) - 0 if none
               Allow defender to counter-attack? This prevents infinite loops
                  when two Creature's with counterAttack enabled attack eachother.

            Output:
               (none)
         */
         void attack(Being *defender, Object *weapon, bool allowCounterAttack = true);

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
            Make the Being die.  Triggers beforeDie and afterDie events.

            Input:
               Whether or not to send a message to the console

            Output:
               (none)
         */
         void die(bool showMessage = false);

         /*
            Since there's a lot of code that would otherwise have to be cut and
            pasted to actually do the respawn() and send a message to the
            terminal, I'm putting it here in a common function.

            Input:
               (none)

            Output:
               (none)
         */
         void doRespawn();

         /*
            The opposite of die :)  Triggers beforeRespawn and afterRespawn
            events.

            Input:
               (none)

            Output:
               (none)
         */
         void respawn();
   };
}}


#endif

