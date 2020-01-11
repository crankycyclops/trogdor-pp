#include <cmath>

#include <trogdor/entities/being.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/game.h>
#include <trogdor/exception/beingexception.h>


namespace trogdor { namespace entity {


   // Tag is set if the Being is attackable
   const char *Being::AttackableTag = "attackable";

   /***************************************************************************/

   void Being::display(Being *observer, bool displayFull) {

      if (ENTITY_PLAYER == observer->getType()) {

         if (!alive) {

            std::string descDead = getMessage("description_dead");

            if (descDead.length() > 0) {
               observer->out("display") << descDead << std::endl;
            }

            else {
               Entity::display(observer, displayFull);
            }
         }

         else {
            Entity::display(observer, displayFull);
         }
      }
   }

   /***************************************************************************/

   void Being::displayShort(Being *observer) {

      if (ENTITY_PLAYER == observer->getType()) {

         if (!alive) {

            observer->out("display") << "You see the corpse of " << getTitle() << '.';

            std::string descDead = getMessage("descshort_dead");

            if (descDead.length() > 0) {
               observer->out("display") << ' ' << descDead;
            }

            observer->out("display") << std::endl;
         }

         else {
            Thing::displayShort(observer);
         }
      }
   }

   /***************************************************************************/

   bool Being::insertIntoInventory(Object *object, bool considerWeight) {

      // make sure the Object will fit
      if (considerWeight && inventory.weight > 0 &&
      inventory.currentWeight + object->getWeight() > inventory.weight) {
         return false;
      }

      // insert the object into the Being's inventory
      inventory.objects.insert(object);
      inventory.currentWeight += object->getWeight();

      // allow referencing of inventory Objects by name and aliases
      std::vector<std::string> objAliases = object->getAliases();
      for (int i = objAliases.size() - 1; i >= 0; i--) {
         indexInventoryItemName(objAliases[i], object);
      }

      inventory.count++;
      object->setOwner(this);

      if (isType(ENTITY_CREATURE) && object->isTagSet(Object::WeaponTag)) {
         static_cast<Creature *>(this)->clearWeaponCache();
      }

      return true;
   }

   /***************************************************************************/

   void Being::removeFromInventory(Object *object) {

      inventory.objects.erase(object);

      std::vector<std::string> objAliases = object->getAliases();
      for (int i = objAliases.size() - 1; i >= 0; i--) {
         inventory.objectsByName.find(objAliases[i])->second.remove(object);
      }

      inventory.count--;
      inventory.currentWeight -= object->getWeight();
      object->setOwner(0);

      if (isType(ENTITY_CREATURE) && object->isTagSet(Object::WeaponTag)) {
         static_cast<Creature *>(this)->clearWeaponCache();
      }
   }

   /***************************************************************************/

   void Being::gotoLocation(Place *l) {

      EventArgumentList eventArgs;

      // this argument will be ignored by Lua events
      eventArgs.push_back(game);

      eventArgs.push_back(this);
      eventArgs.push_back(location);
      eventArgs.push_back(l);

      game->setupEventHandler();
      game->addEventListener(l->getEventListener());
      game->addEventListener(triggers.get());
      if (!game->event("beforeGotoLocation", eventArgs)) {
         return;
      }

      Place *oldLoc = location;

      if (0 != oldLoc) {
         oldLoc->removeThing(this);
      }

      // I do this first, and the other message second so that the Being that's
      // leaving won't see messages about its own departure and arrival ;)
      l->out("notifications") << getTitle() << " arrives." << std::endl;

      l->insertThing(this);
      setLocation(l);
      l->observe(this);

      oldLoc->out("notifications") << getTitle() << " leaves." << std::endl;

      game->setupEventHandler();
      game->addEventListener(l->getEventListener());
      game->addEventListener(triggers.get());
      game->event("afterGotoLocation", eventArgs);
   }

   /***************************************************************************/

   void Being::take(Object *object, bool checkUntakeable, bool doEvents) {

      EventArgumentList eventArgs;

      if (doEvents) {
         eventArgs.push_back(this);
         eventArgs.push_back(object);

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(object->getEventListener());
         if (!game->event("beforeTake", eventArgs)) {
            return;
         }
      }

      if (checkUntakeable && object->isTagSet(Object::UntakeableTag)) {

         if (doEvents) {
            game->setupEventHandler();
            game->addEventListener(triggers.get());
            game->addEventListener(object->getEventListener());
            game->event("takeUntakeable", eventArgs);
         }

         throw BeingException("", BeingException::TAKE_UNTAKEABLE);
      }

      if (!insertIntoInventory(object)) {

         if (doEvents) {
            game->setupEventHandler();
            game->addEventListener(triggers.get());
            game->addEventListener(object->getEventListener());
            game->event("takeTooHeavy", eventArgs);
         }

         throw BeingException("", BeingException::TAKE_TOO_HEAVY);
      }

      else {
         object->getLocation()->out("notifications") << getTitle() << " takes "
            << object->getTitle() << "." << std::endl;
         object->getLocation()->removeThing(object);
      }

      if (doEvents) {
         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(object->getEventListener());
         game->event("afterTake", eventArgs);
      }
   }

   /***************************************************************************/

   void Being::drop(Object *object, bool checkUndroppable, bool doEvents) {

      EventArgumentList eventArgs;

      if (doEvents) {

         eventArgs.push_back(this);
         eventArgs.push_back(object);

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(object->getEventListener());
         if (!game->event("beforeDrop", eventArgs)) {
            return;
         }
      }

      if (checkUndroppable && object->isTagSet(Object::UndroppableTag)) {

         if (doEvents) {
            game->setupEventHandler();
            game->addEventListener(triggers.get());
            game->addEventListener(object->getEventListener());
            game->event("dropUndroppable", eventArgs);
         }

         throw BeingException("", BeingException::DROP_UNDROPPABLE);
      }

      location->out("notifications") << getTitle() << " drops "
         << object->getTitle() << "." << std::endl;
      location->insertThing(object);
      removeFromInventory(object);

      if (doEvents) {
         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(object->getEventListener());
         game->event("afterDrop", eventArgs);
      }
   }

   /***************************************************************************/

   int Being::calcDamage(Being *defender, Object *weapon) {

      int damage;

      damage = round(damageBareHands * getAttributeFactor("strength"));

      // make sure we always do at least 1 point damage
      damage = damage > 0 ? damage : 1;

      if (0 != weapon && weapon->isTagSet(Object::WeaponTag)) {
         damage += weapon->getDamage();
      }

      damage *= defender->getDamageRatio();
      return damage;
   }

   /***************************************************************************/

   // TODO: I *think* I used this in order to account for defense, like armor...
   double Being::getDamageRatio() {

      // TODO: this doesn't actually do anything yet...
      return 1;
   }

   /***************************************************************************/

   void Being::attack(Being *defender, Object *weapon, bool allowCounterAttack) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(defender);
      eventArgs.push_back(weapon);

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      game->addEventListener(defender->getEventListener());
      if (0 != weapon) {
         game->addEventListener(weapon->getEventListener());
      }

      if (!game->event("beforeAttack", eventArgs)) {
         return;
      }

      if (!isAlive()) {

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(defender->getEventListener());
         if (0 != weapon) {
            game->addEventListener(weapon->getEventListener());
         }

         if (!game->event("attackAggressorAlreadyDead", eventArgs)) {
            return;
         }

         out("combat") << "You're already dead and cannot fight." << std::endl;
         return;
      }

      if (!defender->isAlive()) {

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(defender->getEventListener());
         if (0 != weapon) {
            game->addEventListener(weapon->getEventListener());
         }

         if (!game->event("attackDefenderAlreadyDead", eventArgs)) {
            return;
         }

         out("combat") << defender->getTitle() << " is already dead." << std::endl;
         return;
      }

      // if defender is immortal, then there's really no point...
      if (defender->isImmortal()) {

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(defender->getEventListener());
         if (0 != weapon) {
            game->addEventListener(weapon->getEventListener());
         }

         if (!game->event("attackDefenderIsImmortal", eventArgs)) {
            return;
         }

         out("combat") << defender->getTitle()
            << " is immortal and cannot die." << std::endl;
         return;
      }

      // Defender isn't attackable
      else if (!defender->isTagSet(Being::AttackableTag)) {

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(defender->getEventListener());
         if (0 != weapon) {
            game->addEventListener(weapon->getEventListener());
         }

         if (!game->event("attackDefenderNotAttackable", eventArgs)) {
            return;
         }

         out("combat") << defender->getTitle()
            << " cannot be attacked." << std::endl;
         return;
      }

      // used either if the attack is successful or if it fails
      game->setupEventHandler();
      game->addEventListener(triggers.get());
      game->addEventListener(defender->getEventListener());
      if (0 != weapon) {
         game->addEventListener(weapon->getEventListener());
      }

      // send notification to the aggressor
      out("combat") << "You attack " << defender->getTitle();

      if (0 != weapon) {
         out("combat") << " with " << weapon->getTitle();
      }

      out("combat") << '.' << std::endl;

      // send notification to the defender
      defender->out("combat") << "You're attacked by " << getTitle();

      if (0 != weapon) {
         defender->out("combat") << " with " << weapon->getTitle();
      }

      defender->out("combat") << '!' << std::endl;

      if (isAttackSuccessful(defender)) {

         if (!game->event("attackSuccess", eventArgs)) {
            return;
         }

         int damage = calcDamage(defender, weapon);

         defender->removeHealth(damage);

         out("combat") << "You dealt a blow to " << defender->getTitle() << "!" << std::endl;
         defender->out("combat") << getTitle() << " dealt you a blow!" << std::endl;
         out("combat") << defender->getTitle() << " loses " << damage <<
            " health points." << std::endl;
         defender->out("combat") << "You lose " << damage << " health points." << std::endl;

         if (!defender->isAlive()) {
            return;
         }
      }

      else {

         if (!game->event("attackFailure", eventArgs)) {
            return;
         }

         out("combat") << "Your attack failed." << std::endl;
         defender->out("combat") << getTitle() << "'s attack failed." << std::endl;
      }

      if (
         ENTITY_CREATURE == defender->getType() &&
         Creature::FRIEND != static_cast<Creature *>(defender)->getAllegiance() &&
         static_cast<Creature *>(defender)->getCounterAttack() &&
         allowCounterAttack
      ) {
         defender->attack(this, static_cast<Creature *>(defender)->selectWeapon(), false);
      }

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      game->addEventListener(defender->getEventListener());
      if (0 != weapon) {
         game->addEventListener(weapon->getEventListener());
      }

      game->event("afterAttack", eventArgs);
   }

   /***************************************************************************/

   void Being::addHealth(int up, bool allowOverflow) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(health);
      eventArgs.push_back(up);

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      if (!game->event("beforeAddHealth", eventArgs)) {
         return;
      }

      health += up;

      if (!allowOverflow && health > maxHealth) {
         health = maxHealth;
      }

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      game->event("afterAddHealth", eventArgs);
   }

   /***************************************************************************/

   void Being::removeHealth(int down, bool allowDeath) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(health);
      eventArgs.push_back(down);

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      if (!game->event("beforeRemoveHealth", eventArgs)) {
         return;
      }

      health -= down;

      if (health <= 0) {

         health = 0; // just in case it's a negative number, we clip at 0

         if (allowDeath) {
            die(true);
         }
      }

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      game->event("afterRemoveHealth", eventArgs);
   }

   /***************************************************************************/

   void Being::die(bool showMessage) {

      EventArgumentList eventArgs;

      // the game argument will be ignored by Lua events
      eventArgs.push_back(game);
      eventArgs.push_back(this);

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      if (!game->event("beforeDie", eventArgs)) {
         return;
      }

      alive = false;

      if (showMessage) {
         getLocation()->out("notifications") << title << " dies." << std::endl;
      }

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      game->event("afterDie", eventArgs);
   }

   /***************************************************************************/

   void Being::doRespawn() {

      if (!isAlive()) {

         respawn();

         // it's possible that a respawn event prevented the Being from coming
         // back to life, so make sure it did before we send out the message
         if (isAlive()) {

            std::string msg = getMessage("respawn");

            if (0 == msg.length()) {
               msg = name + " comes back to life.";
            }

            getLocation()->out("notifications") << std::endl << msg << std::endl;
         }
      }
   }

   /***************************************************************************/

   void Being::respawn() {

      EventArgumentList eventArgs;

      // the game argument will be ignored by Lua events
      eventArgs.push_back(game);
      eventArgs.push_back(this);

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      if (!game->event("beforeRespawn", eventArgs)) {
         return;
      }

      alive = true;
      health = maxHealth;

      game->setupEventHandler();
      game->addEventListener(triggers.get());
      game->event("afterRespawn", eventArgs);
   }
}}
