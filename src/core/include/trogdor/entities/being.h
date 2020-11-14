#ifndef BEING_H
#define BEING_H


#include <set>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <random>

#include <trogdor/utility.h>

#include <trogdor/entities/thing.h>
#include <trogdor/entities/place.h>
#include <trogdor/entities/object.h>

#include <trogdor/exception/undefinedexception.h>


namespace trogdor::entity {


   class Being: public Thing {

      // addAlias needs to be able to call indexInventoryItemName()
      friend void Object::addAlias(std::string alias);

      public:

         // Tag is set if the Being is attackable
         static constexpr const char *AttackableTag = "attackable";

         static constexpr int DEFAULT_ATTRIBUTE_STRENGTH     = 10;
         static constexpr int DEFAULT_ATTRIBUTE_DEXTERITY    = 10;
         static constexpr int DEFAULT_ATTRIBUTE_INTELLIGENCE = 10;

         static constexpr bool DEFAULT_RESPAWN_ENABLED = false;
         static constexpr int DEFAULT_RESPAWN_INTERVAL = 0;
         static constexpr int DEFAULT_RESPAWN_LIVES = -1;

         static constexpr int DEFAULT_INVENTORY_WEIGHT = 0;

         static constexpr bool DEFAULT_ATTACKABLE = true;
         static constexpr int DEFAULT_DAMAGE_BARE_HANDS = 5;

         // By default, Beings are immortal (a maximum health needs to be
         // set explicitly)
         static constexpr int DEFAULT_MAX_HEALTH = 0;

      private:

         // If Being is dropping an ephemeral Resource, we call this to free the
         // allocation.
         inline void freeResource(
            Resource *resource,
            double amount,
            bool doEvents = true
         ) {

            if (doEvents && !game->event({
               "beforeDropResource",
               {triggers.get(), resource->getEventListener()},
               {this, resource, amount}
            })) {
               return;
            }

            std::string message = resource->getMessage("drop");

            switch(resource->free(getShared(), amount)) {

               case entity::Resource::ALLOCATE_OR_FREE_SUCCESS:

                  game->event({
                     "afterDropResource",
                     {triggers.get(), resource->getEventListener()},
                     {this, resource, amount}
                  });

                  if (message.length() > 0) {
                     out("display") << message << std::endl;
                  } else {
                     out("display") << "You drop "
                        << resource->amountToString(amount) << ' '
                        << resource->titleToString(amount) << '.' << std::endl;
                  }

                  break;

               // If the action was aborted due to an event handler, we'll let
               // that event send appropriate feedback to the player
               case entity::Resource::ALLOCATE_OR_FREE_ABORT:

                  break;

               // An error occurred :(
               default:

                  if (resource->getMessage("cantDrop").length()) {
                     out("display") << resource->getMessage("cantDrop")
                        << std::endl;
                  }

                  else {
                     out("display") << "You can't drop that!" << std::endl;
                  }

                  break;
            }
         }

         // If Being is dropping a Resource that isn't ephemeral, we call this
         // to transfer the allocation to the Place the Being occupies.
         inline void transferResourceToPlace(
            Resource *resource,
            double amount,
            bool doEvents = true
         ) {

            if (auto location = getLocation().lock()) {

               if (doEvents && !game->event({
                  "beforeDropResource",
                  {triggers.get(), resource->getEventListener()},
                  {this, resource, amount}
               })) {
                  return;
               }

               switch (resource->transfer(getShared(), location, amount)) {

                  // Yay!
                  case entity::Resource::ALLOCATE_OR_FREE_SUCCESS:

                     game->event({
                        "afterDropResource",
                        {triggers.get(), resource->getEventListener()},
                        {this, resource, amount}
                     });

                     // Notify every entity in the room that the resource has
                     // been dropped EXCEPT the one who's doing the dropping.
                     for (auto const &thing: location->getThings()) {
                        if (thing.get() != this) {
                           thing->out("notifications")
                              << getProperty<std::string>(TitleProperty)
                              << " drops " << resource->amountToString(amount) << ' '
                              << resource->titleToString(amount) << "." << std::endl;
                        }
                     };

                     if (resource->getMessage("drop").length()) {
                        out("display") << resource->getMessage("drop")
                           << std::endl;
                     } else {
                        out("display") << "You drop "
                           << resource->amountToString(amount) << ' '
                           << resource->titleToString(amount) << '.' << std::endl;
                     }

                     break;

                  // The room has alrady reached max capacity for this resource
                  // and can't take anymore
                  case entity::Resource::ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED:

                     if (resource->getMessage("dropErrorPlaceFull").length()) {
                        out("display") << resource->getMessage("dropErrorPlaceFull")
                           << std::endl;
                     } else {
                     out("display") << "This place can only hold "
                        << resource->amountToString(*resource->getMaxAmountPerDepositor())
                        << ' ' << resource->titleToString(*resource->getMaxAmountPerDepositor())
                        << '.' << std::endl;
                     }

                     break;

                  // If the action was aborted due to an event handler, we'll let
                  // that event send appropriate feedback to the player
                  case entity::Resource::ALLOCATE_OR_FREE_ABORT:

                     break;

                  // An error occurred :(
                  default:

                     if (resource->getMessage("cantDrop").length()) {
                        out("display") << resource->getMessage("cantDrop")
                           << std::endl;
                     }

                     else {
                        out("display") << "You can't drop that!" << std::endl;
                     }

                     break;
               }
            }
         }

      protected:

         // Once the value for health has been initialized, set this to true.
         // This is used to determine whether setMaxHealth should also
         // initialize the value of health.
         bool healthInitialized = false;

         int health = 0;        // number of health points the being currently has
         int maxHealth = 0;     // maximum number of health points (0 for immortal)

         double woundRate;      // max probability of being hit when attacked
         int damageBareHands;   // damage done during combat without a weapon

         struct {
            bool enabled;
            int  interval;
            int  lives;
         } respawnSettings;

         struct {
            std::unordered_map<std::string, int> values;
            int initialTotal;   // total attributes that the Being started with
         } attributes;

         struct {

            int weight;         // how much weight inventory can hold
            int currentWeight;  // how much weight is currently used
            unsigned count;     // number of objects in the inventory

            // Inventory items
            std::set<std::shared_ptr<Object>, EntityAlphaComparator> objects;

            // Inventory items indexed by alias
            std::unordered_map<std::string, std::list<Object *>> objectsByName;
         } inventory;

         /*
            Overrides Tangible::display() in order to handle the description of
            living vs. deceased Beings.

            Input:
               Being observing the Place
               Whether or not to always show the long description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

         /*
            Overrides Tangible::displayShort in order to handle the short
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
               Alias (std::string)
               Object to be indexed (Object *)

            Output:
               (none)
         */
         inline void indexInventoryItemName(std::string alias, Object *object) {

            mutex.lock();

            if (inventory.objectsByName.find(alias) == inventory.objectsByName.end()) {
               inventory.objectsByName[alias] = {};
            }

            inventory.objectsByName.find(alias)->second.push_back(object);
            mutex.unlock();
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
         bool isAttackSuccessful(Being *defender);

      public:

         /*
            Constructor for creating a new Being.  Requires reference to the
            containing Game object and a name.
         */
         Being(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            Constructor that clones a Being into another separate and unique
            Being with identical properties.  Requires a unique name, which
            won't be copied.

            NOTE: inventory objects will NOT be copied, in order to maintain
            sanity -- it doesn't make sense for two Beings to simultaneously
            carry the same object, right? ;)
         */
         Being(const Being &b, std::string n);

         /*
            Returns a smart pointer representing a raw Being pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Being>
         */
         inline std::shared_ptr<Being> getShared() {

            return std::dynamic_pointer_cast<Being>(Entity::getShared());
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
         inline bool isAlive() const {return health || !maxHealth ? true : false;}

         /*
            Returns true if the Being is immortal and false if it's not.

            Input:
               (none)

            Output:
               bool
         */
         inline bool isImmortal() const {return maxHealth == 0 ? true : false;}

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
            Returns all objects in the Being's inventory.

            Input:
               (none)

            Output:
               Set of objects in the inventory
         */
         inline const auto &getInventoryObjects() const {return inventory.objects;}

         /*
            Returns iterable list of all Objects that match the given name in
            the Being's inventory.

            Input:
               name (std::string)

            Output:
               List of all inventory objects matching the given name
         */
         inline const auto &getInventoryObjectsByName(std::string name) const {

            ObjectsByNameMap::const_iterator i = inventory.objectsByName.find(name);

            if (i == inventory.objectsByName.end()) {
               return emptyObjectList;
            }

            else {
               return i->second;
            }
         }

         /*
            Return raw point value for an attribute.

            Input:
               Name of the attribute whose value you wish to retrieve

            Output:
               Attribute value in points (int)
         */
         inline int getAttribute(std::string key) const {

            if (attributes.values.find(key) == attributes.values.end()) {
               throw UndefinedException(std::string("attribute '") + key + "' not set!");
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
         inline double getAttributeFactor(std::string key) const {

            return (double)getAttribute(key) / (double)attributes.initialTotal;
         }

         /*
            Return the Being's current health.

            Input:
               (none)

            Ouput:
               Health (int)
         */
         inline int getHealth() {return health;}

         /*
            Return the Being's maximum health.

            Input:
               (none)

            Ouput:
               Maximum Health (int)
         */
         inline int getMaxHealth() {return maxHealth;}

         /*
            Send out a JSON update every time the player's health is changed.

            Input:
               (none)

            Output:
               (none)
         */
         inline void notifyHealth() {

            out("health") << std::string("{\"health\":") + std::to_string(health) +
               ",\"maxHealth\":" + std::to_string(maxHealth) + '}';
            out("health").flush();
         }

         /*
            Sets the Being's health.

            Input:
               Integer number of health points

            Output:
               (none)
         */
         inline void setHealth(int h) {

            // TODO: should I intelligently handle player dying here instead of
            // in the separate method die()? I think that makes more sense
            mutex.lock();
            healthInitialized = true;
            health = h;
            mutex.unlock();

            notifyHealth();
         }

         /*
            Sets the Being's maximum health (0 for immortal.)

            Input:
               Integer number of health points

            Output:
               (none)
         */
         inline void setMaxHealth(int h) {

            // If we haven't already initialized the value for health, do so now
            // (the default value is the same value as maxHealth.)
            if (!healthInitialized) {
               setHealth(h);
            }

            mutex.lock();
            maxHealth = h;
            mutex.unlock();
         }

         /*
            Sets Being's wound rate, which is a factor in how likely it is to be
            hit during an attack.

            Input:
               Double

            Output:
               (none)
         */
         inline void setWoundRate(double rate) {

            mutex.lock();
            woundRate = rate;
            mutex.unlock();
         }

         /*
            Sets the amount of damage done by the Being's bare hands.

            Input:
               Int

            Output:
               (none)
         */
         inline void setDamageBareHands(int d) {

            mutex.lock();
            damageBareHands = d;
            mutex.unlock();
         }

         /*
            Setters for respawn settings.

            Input:
               values (bool or int)

            Output:
               (none)
         */
         inline void setRespawnEnabled(bool b) {

            mutex.lock();
            respawnSettings.enabled = b;
            mutex.unlock();
         }

         inline void setRespawnInterval(int i) {

            mutex.lock();
            respawnSettings.interval = i;
            mutex.unlock();
         }

         inline void setRespawnLives(int i) {

            mutex.lock();
            respawnSettings.lives = i;
            mutex.unlock();
         }

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
               mutex.lock();
               respawnSettings.lives++;
               mutex.unlock();
            }
         }

         /*
            Sets an attribute.

            Input:
               Name of the attribute (std::string)
               Attribute value (int)

            Output:
               (none)
         */
         inline void setAttribute(std::string key, int s) {

            mutex.lock();
            attributes.values[key] = s;
            mutex.unlock();
         }

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

            mutex.lock();

            attributes.initialTotal = 0;

            for (const auto &attribute: attributes.values) {
               attributes.initialTotal += attribute.second;
            }

            mutex.unlock();
         }

         /*
            Sets the inventory's weight.  0 means the Being  can carry an
            unlimited number of items.

            Input:
               Weight (int)

            Output:
               (none)
         */
         inline void setInventoryWeight(int w) {

            mutex.lock();
            inventory.weight = w;
            mutex.unlock();
         }

         /*
            Inserts the given object into the Being's inventory.  Returns true
            if the Object fits (there's enough free weight.)  Otherwise, nothing
            happens and false is returned.  By default, we check the inventory's
            available weight before inserting, but passing false to the second
            argument will bypass this check (should only be used during the
            parsing of game.xml.)

            Input:
               const std::shared_ptr<Object> &
               Whether or not to check the inventory's weight before inserting (default = true)

            Output:
               bool (true on success and false on failure)
         */
         virtual bool insertIntoInventory(
            const std::shared_ptr<Object> &object,
            bool considerWeight = true
         );

         /*
            Removes an object from a Being's inventory.  If the Object isn't in
            the Being's inventory, nothing happens.

            Input:
               Pointer to Object to remove

            Output:
               (none)
         */
         virtual void removeFromInventory(const std::shared_ptr<Object> &object);

         /*
            Make the Being move to the specified Place.  This both sets the
            Being's location and has the Being observe the Place.  Events are
            triggered for both actions.

            Input:
               Place where Being should go

            Output:
               (none)
         */
         void gotoLocation(const std::shared_ptr<Place> &location);

         /*
            Allows a Being to take an object.  Calls insertIntoInventory() and
            triggers the appropriate events.

            Input:
               Object to take
               Whether or not to check for the taking of "untakeable objects"
               Whether or not to trigger before and after take events

            Output:
               (none)
         */
         void take(
            const std::shared_ptr<Object> &object,
            bool checkUntakeable = true,
            bool doEvents = true
         );

         /*
            Allows a Being to take some amount of a resource.

            Input:
               Resource to take (const std::shared_ptr<Resource> &)
               Amount to take (double)
               Whether or not to trigger before and after take events (bool)

            Output:
               (none)
         */
         void takeResource(
            const std::shared_ptr<Resource> &resource,
            double amount,
            bool doEvents = true
         );

         /*
            Allows a Being to drop an object.  Calls removeIntoInventory() and
            triggers the appropriate events.

            Input:
               Object to drop
               Whether or not to check for the dropping of "undroppable objects"
               Whether or not to trigger before and after drop events

            Output:
               (none)
         */
         void drop(
            const std::shared_ptr<Object> &object,
            bool checkUndroppable = true,
            bool doEvents = true
         );

         /*
            Allows a Being to drop some amount of a resource.

            Input:
               Resource to drop (const std::shared_ptr<Resource> &)
               Amount to drop (double)
               Whether or not to trigger before and after drop events (bool)

            Output:
               (none)
         */
         void dropResource(
            const std::shared_ptr<Resource> &resource,
            double amount,
            bool doEvents = true
         );

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
}


#endif
