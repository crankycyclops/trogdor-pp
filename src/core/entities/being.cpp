#include <cmath>
#include <algorithm>

#include <trogdor/entities/being.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/game.h>
#include <trogdor/exception/beingexception.h>


namespace trogdor::entity {


   // Tag is set if the Being is attackable
   const char *Being::AttackableTag = "attackable";

   /***************************************************************************/

   Being::Being(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Thing(g, n,
   std::move(o), std::move(i), std::move(e)),
   maxHealth(DEFAULT_MAX_HEALTH),
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

      if (DEFAULT_ATTACKABLE) {
         setTag(AttackableTag);
      }
   }

   /***************************************************************************/

   Being::Being(const Being &b, std::string n): Thing(b, n) {

      setHealth(b.health);
      setMaxHealth(b.maxHealth);
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

   /***************************************************************************/

   void Being::display(Being *observer, bool displayFull) {

      if (ENTITY_PLAYER == observer->getType()) {

         if (!isAlive()) {

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

         if (!isAlive()) {

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

   bool Being::insertIntoInventory(const std::shared_ptr<Object> &object, bool considerWeight) {

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
         indexInventoryItemName(objAliases[i], object.get());
      }

      inventory.count++;
      object->setOwner(getShared());

      if (isType(ENTITY_CREATURE) && object->isTagSet(Object::WeaponTag)) {
         static_cast<Creature *>(this)->clearWeaponCache();
      }

      return true;
   }

   /***************************************************************************/

   void Being::removeFromInventory(const std::shared_ptr<Object> &object) {

      std::vector<std::string> objAliases = object->getAliases();
      for (int i = objAliases.size() - 1; i >= 0; i--) {
         inventory.objectsByName.find(objAliases[i])->second.remove(object.get());
      }

      inventory.objects.erase(object);

      inventory.count--;
      inventory.currentWeight -= object->getWeight();
      object->setOwner(std::weak_ptr<Being>());

      if (isType(ENTITY_CREATURE) && object->isTagSet(Object::WeaponTag)) {
         static_cast<Creature *>(this)->clearWeaponCache();
      }
   }

   /***************************************************************************/

   void Being::gotoLocation(const std::shared_ptr<Place> &l) {

      auto oldLoc = location.lock();
      std::vector<event::EventArgument> eventArgs = {game, this};

      if (oldLoc) {
         eventArgs.push_back(oldLoc.get());
      }

      eventArgs.push_back(l.get());

      if (!game->event({
         "beforeGotoLocation",
         {l->getEventListener(), triggers.get()},
         eventArgs
      })) {
         return;
      }

      if (oldLoc) {
         oldLoc->removeThing(getShared());
      }

      // I do this first, and the other message second so that the Being that's
      // leaving won't see messages about its own departure and arrival ;)
      l->out("notifications") << getTitle() << " arrives." << std::endl;

      l->insertThing(getShared());
      setLocation(l);
      l->observe(this);

      oldLoc->out("notifications") << getTitle() << " leaves." << std::endl;

      game->event({
         "afterGotoLocation",
         {l->getEventListener(), triggers.get()},
         eventArgs
      });
   }

   /***************************************************************************/

   void Being::take(
      const std::shared_ptr<Object> &object,
      bool checkUntakeable,
      bool doEvents
   ) {

      if (auto location = object->getLocation().lock()) {

         if (doEvents && !game->event({
            "beforeTake",
            {triggers.get(), object->getEventListener()},
            {this, object.get()}
         })) {
            return;
         }

         if (checkUntakeable && object->isTagSet(Object::UntakeableTag)) {

            if (doEvents) {
               game->event({
                  "takeUntakeable",
                  {triggers.get(), object->getEventListener()},
                  {this, object.get()}
               });
            }

            throw BeingException("", BeingException::TAKE_UNTAKEABLE);
         }

         if (!insertIntoInventory(object)) {

            if (doEvents) {
               game->event({
                  "takeTooHeavy",
                  {triggers.get(), object->getEventListener()},
                  {this, object.get()}
               });
            }

            throw BeingException("", BeingException::TAKE_TOO_HEAVY);
         }

         else {

            // Notify every entity in the room that the object has been taken
            // EXCEPT the one who's doing the taking
            for (auto const &thing: location->getThings()) {
               if (thing.get() != this) {
                  thing->out("notifications") << getTitle() << " takes "
                     << object->getTitle() << "." << std::endl;
               }
            };

            location->removeThing(object);
         }

         if (doEvents) {
            game->event({
               "afterTake",
               {triggers.get(), object->getEventListener()},
               {this, object.get()}
            });
         }
      }
   }

   /***************************************************************************/

   void Being::drop(
      const std::shared_ptr<Object> &object,
      bool checkUndroppable,
      bool doEvents
   ) {

      if (auto location = object->getLocation().lock()) {

         if (doEvents && !game->event({
            "beforeDrop",
            {triggers.get(), object->getEventListener()},
            {this, object.get()}
         })) {
            return;
         }

         if (checkUndroppable && object->isTagSet(Object::UndroppableTag)) {

            if (doEvents) {
               game->event({
                  "dropUndroppable",
                  {triggers.get(), object->getEventListener()},
                  {this, object.get()}
               });
            }

            throw BeingException("", BeingException::DROP_UNDROPPABLE);
         }

         // Notify every entity in the room that the object has been dropped EXCEPT
         // the one who's doing the dropping
         for (auto const &thing: location->getThings()) {
            if (thing.get() != this) {
               thing->out("notifications") << getTitle() << " drops "
                  << object->getTitle() << "." << std::endl;
            }
         };

         location->insertThing(object);
         removeFromInventory(object);

         if (doEvents) {
            game->event({
               "afterDrop",
               {triggers.get(), object->getEventListener()},
               {this, object.get()}
            });
         }
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

   double Being::getDamageRatio() {

      // TODO: this doesn't actually do anything yet...
      return 1;
   }

   /***************************************************************************/

   bool Being::isAttackSuccessful(Being *defender) {

      static std::random_device rd;
      static std::mt19937 generator(rd());
      static std::uniform_real_distribution<double> distribution(0, 1);

      // probability that the attack will be successful
      double p = CLAMP(getAttributeFactor("strength") * (defender->woundRate / 2) +
         (defender->woundRate / 2), 0.0, defender->woundRate);

      if (distribution(generator) < p) {
         return true;
      }

      else {
         return false;
      }
   }

   /***************************************************************************/

   void Being::attack(Being *defender, Object *weapon, bool allowCounterAttack) {

      std::list<event::EventListener *> listeners = {triggers.get(), defender->getEventListener()};
      std::vector<event::EventArgument> args = {this, defender};

      if (0 != weapon) {
         listeners.push_back(weapon->getEventListener());
         args.push_back(weapon);
      }

      if (!game->event({"beforeAttack", listeners, args})) {
         return;
      }

      if (!isAlive()) {

         if (!game->event({"attackAggressorAlreadyDead", listeners, args})) {
            return;
         }

         out("combat") << "You're already dead and cannot fight." << std::endl;
         return;
      }

      if (!defender->isAlive()) {

         if (!game->event({"attackDefenderAlreadyDead", listeners, args})) {
            return;
         }

         out("combat") << defender->getTitle() << " is already dead." << std::endl;
         return;
      }

      // if defender is immortal, then there's really no point...
      if (defender->isImmortal()) {

         if (!game->event({"attackDefenderIsImmortal", listeners, args})) {
            return;
         }

         out("combat") << defender->getTitle()
            << " is immortal and cannot die." << std::endl;
         return;
      }

      // Defender isn't attackable
      else if (!defender->isTagSet(Being::AttackableTag)) {

         if (!game->event({"attackDefenderNotAttackable", listeners, args})) {
            return;
         }

         out("combat") << defender->getTitle()
            << " cannot be attacked." << std::endl;
         return;
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

         if (!game->event({"attackSuccess", listeners, args})) {
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

         if (!game->event({"attackFailure", listeners, args})) {
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

      game->event({"afterAttack", listeners, args});
   }

   /***************************************************************************/

   void Being::addHealth(int up, bool allowOverflow) {

      if (!game->event({"beforeAddHealth", {triggers.get()}, {this, health, up}})) {
         return;
      }

      int tmpHealth = health;
      tmpHealth += up;

      setHealth(!allowOverflow && tmpHealth > maxHealth ? maxHealth : tmpHealth);
      game->event({"afterAddHealth", {triggers.get()}, {this, health, up}});
   }

   /***************************************************************************/

   void Being::removeHealth(int down, bool allowDeath) {

      if (!game->event({"beforeRemoveHealth", {triggers.get()}, {this, health, down}})) {
         return;
      }

      int tmpHealth = health;
      tmpHealth -= down;

      if (tmpHealth <= 0) {

         if (allowDeath) {
            die(true);
         }
      }

      else {
         setHealth(tmpHealth);
      }

      game->event({"afterRemoveHealth", {triggers.get()}, {this, health, down}});
   }

   /***************************************************************************/

   void Being::die(bool showMessage) {

      if (!game->event({"beforeDie", {triggers.get()}, {game, this}})) {
         return;
      }

      // TODO: I need a way to support programmatically killing an immortal
      // player.
      setHealth(0);

      auto location = getLocation().lock();

      if (showMessage && location) {
         location->out("notifications") << title << " dies." << std::endl;
      }

      game->event({"afterDie", {triggers.get()}, {game, this}});
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

            if (auto location = getLocation().lock()) {
               location->out("notifications") << std::endl << msg << std::endl;
            }
         }
      }
   }

   /***************************************************************************/

   void Being::respawn() {

      if (!game->event({"beforeRespawn", {triggers.get()}, {game, this}})) {
         return;
      }

      setHealth(maxHealth);
      game->event({"afterRespawn", {triggers.get()}, {game, this}});
   }
}
