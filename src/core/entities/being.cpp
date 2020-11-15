#include <cmath>
#include <algorithm>

#include <trogdor/entities/being.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/game.h>
#include <trogdor/exception/beingexception.h>


namespace trogdor::entity {


   Being::Being(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Thing(g, n, std::move(o), std::move(e)),
   maxHealth(DEFAULT_MAX_HEALTH) {

      setAttribute("strength", DEFAULT_ATTRIBUTE_STRENGTH);
      setAttribute("dexterity", DEFAULT_ATTRIBUTE_DEXTERITY);
      setAttribute("intelligence", DEFAULT_ATTRIBUTE_INTELLIGENCE);
      setAttributesInitialTotal();

      setProperty(WoundRateProperty, DEFAULT_WOUND_RATE);
      setProperty(DamageBareHandsProperty, DEFAULT_DAMAGE_BARE_HANDS);
      setProperty(RespawnEnabledProperty, DEFAULT_RESPAWN_ENABLED);
      setProperty(RespawnIntervalProperty, DEFAULT_RESPAWN_INTERVAL);
      setProperty(RespawnLivesProperty, DEFAULT_RESPAWN_LIVES);
      setProperty(InvMaxWeightProperty, DEFAULT_INVENTORY_WEIGHT);

      setPropertyValidator(WoundRateProperty, [&](PropertyValue v) -> int {return isPropertyValueDouble(v);});
      setPropertyValidator(DamageBareHandsProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});
      setPropertyValidator(RespawnEnabledProperty, [&](PropertyValue v) -> int {return isPropertyValueBool(v);});
      setPropertyValidator(RespawnIntervalProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});
      setPropertyValidator(RespawnLivesProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});
      setPropertyValidator(InvMaxWeightProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});

      inventory.count         = 0;
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

      attributes.values = b.attributes.values;
      attributes.initialTotal = b.attributes.initialTotal;

      inventory.count = 0;
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
               Tangible::display(observer, displayFull);
            }
         }

         else {
            Tangible::display(observer, displayFull);
         }
      }
   }

   /***************************************************************************/

   void Being::displayShort(Being *observer) {

      if (ENTITY_PLAYER == observer->getType()) {

         if (!isAlive()) {

            observer->out("display") << "You see the corpse of " <<
               getProperty<std::string>(TitleProperty) << '.';

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

   bool Being::insertIntoInventory(
      const std::shared_ptr<Object> &object,
      bool considerWeight
   ) {

      int invMaxWeight = getProperty<int>(InvMaxWeightProperty);
      int objectWeight = object->getProperty<int>(Object::WeightProperty);

      // make sure the Object will fit
      if (considerWeight && invMaxWeight > 0 &&
      inventory.currentWeight + objectWeight > invMaxWeight) {
         return false;
      }

      // insert the object into the Being's inventory
      mutex.lock();
      inventory.objects.insert(object);
      inventory.currentWeight += objectWeight;
      mutex.unlock();

      // allow referencing of inventory Objects by name and aliases
      std::vector<std::string> objAliases = object->getAliases();
      for (int i = objAliases.size() - 1; i >= 0; i--) {
         indexInventoryItemName(objAliases[i], object.get());
      }

      mutex.lock();
      inventory.count++;
      object->setOwner(getShared());
      mutex.unlock();

      return true;
   }

   /***************************************************************************/

   void Being::removeFromInventory(const std::shared_ptr<Object> &object) {

      mutex.lock();

      std::vector<std::string> objAliases = object->getAliases();
      for (int i = objAliases.size() - 1; i >= 0; i--) {
         inventory.objectsByName.find(objAliases[i])->second.remove(object.get());
      }

      inventory.objects.erase(object);

      inventory.count--;
      inventory.currentWeight -= object->getProperty<int>(Object::WeightProperty);
      object->setOwner(std::weak_ptr<Being>());

      mutex.unlock();
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
      l->out("notifications") << getProperty<std::string>(TitleProperty)
         << " arrives." << std::endl;

      l->insertThing(getShared());
      setLocation(l);
      l->observe(getShared());

      oldLoc->out("notifications") << getProperty<std::string>(TitleProperty)
         << " leaves." << std::endl;

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

            out("display") << "You can't take that!" << std::endl;
            return;
         }

         if (!insertIntoInventory(object)) {

            if (doEvents) {
               game->event({
                  "takeTooHeavy",
                  {triggers.get(), object->getEventListener()},
                  {this, object.get()}
               });
            }

            out("display") << object->getName()
               << " is too heavy.  Try dropping something first." << std::endl;
            return;
         }

         else {

            location->removeThing(object);

            // Notify every entity in the room that the object has been taken
            // EXCEPT the one who's doing the taking
            for (auto const &thing: location->getThings()) {
               if (thing.get() != this) {
                  thing->out("notifications") <<
                     getProperty<std::string>(TitleProperty) << " takes "
                     << object->getProperty<std::string>(TitleProperty)
                     << "." << std::endl;
               }
            };

            std::string message = object->getMessage("take");

            if (message.length() > 0) {
               out("display") << message << std::endl;
            } else {
               out("display") << "You take the " << object->getName()
                  << "." << std::endl;
            }
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

   void Being::takeResource(
      const std::shared_ptr<Resource> &resource,
      double amount,
      bool doEvents
   ) {

      if (auto location = getLocation().lock()) {

         if (doEvents && !game->event({
            "beforeTakeResource",
            {triggers.get(), resource->getEventListener()},
            {this, resource.get(), amount}
         })) {
            return;
         }

         // If we find a resource, make sure a process in another thread doesn't
         // remove the allocation before we've had the chance to read its value.
         mutex.lock();

         if (location->getResources().end() != location->getResources().find(resource)) {

            entity::Resource::AllocationStatus status;
            double allocatedToPlace = location->getResources().find(resource)->second;

            mutex.unlock();

            // If the resource is "sticky" and can be allocated in
            // unlimited amounts, the room should keep what it has and
            // we should allocate extra (as long as it's less than or
            // equal to the amount already in the room.)
            if (
               resource->isTagSet(Resource::StickyTag) &&
               !resource->isPropertySet(Resource::AmtAvailProperty)
            ) {

               if (amount > allocatedToPlace) {
                  out("display") << "You can only take "
                     << resource->amountToString(allocatedToPlace)
                     << ' ' << resource->getProperty<std::string>(Resource::PluralTitleProperty)
                     << '.' << std::endl;
                  return;
               }

               status = resource->allocate(getShared(), amount);
            }

            else {
               status = resource->transfer(location, getShared(), amount);
            }

            std::string message = resource->getMessage("take");

            switch (status) {

               // Transfers must be made in integer amounts
               case entity::Resource::FREE_INT_REQUIRED:
               case entity::Resource::ALLOCATE_INT_REQUIRED:

                  if (resource->getMessage("takeErrorIntRequired").length()) {
                     out("display") << resource->getMessage("takeErrorIntRequired")
                        << std::endl;
                  } else {
                     out("display") << "Please specify a whole number of "
                        << resource->getProperty<std::string>(Resource::PluralTitleProperty)
                        << '.' << std::endl;
                  }

                  break;

               // The transfer would result in the player possessing
               // more of the resource than they're allowed to have
               case entity::Resource::ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED:

                  if (resource->getMessage("takeErrorMaxExceeded").length()) {
                     out("display") << resource->getMessage("takeErrorMaxExceeded")
                        << std::endl;
                  } else {
                     out("display") << "That would give you "
                        << resource->amountToString(getResources().find(resource)->second + amount)
                        << ' ' << resource->getProperty<std::string>(Resource::PluralTitleProperty)
                        << " and you're only allowed to possess "
                        << resource->amountToString(resource->getProperty<double>(Resource::MaxAmtPerDepositorProperty))
                        << '.' << std::endl;
                  }

                  break;

               // We can't transfer an amount less than or equal to 0
               case entity::Resource::FREE_NEGATIVE_VALUE:
               case entity::Resource::ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT:

                  if (resource->getMessage("takeErrorInvalidValue").length()) {
                     out("display") << resource->getMessage("takeErrorInvalidValue")
                        << std::endl;
                  } else {
                     out("display") << "Please specify an amount greater than zero."
                        << std::endl;
                  }

                  break;

               // The original holder of the resource doesn't have the
               // requested amount
               case entity::Resource::FREE_EXCEEDS_ALLOCATION:

                  if (resource->getMessage("takeErrorRequestTooMuch").length()) {
                     out("display") << resource->getMessage("takeErrorRequestTooMuch")
                        << std::endl;
                  } else {
                     out("display") << "You can only take "
                        << resource->amountToString(allocatedToPlace)
                        << ' ' << resource->getProperty<std::string>(Resource::PluralTitleProperty)
                        << '.' << std::endl;
                  }

                  break;

               // We already check for this condition above, but checking this
               // case squashes a compiler warning, so I'm leaving it here, even
               // if it's redundant.
               case entity::Resource::ALLOCATE_TOTAL_AMOUNT_EXCEEDED:

                  out("display") << "You can only take "
                     << resource->amountToString(allocatedToPlace)
                     << ' ' << resource->getProperty<std::string>(Resource::PluralTitleProperty)
                     << '.' << std::endl;

                  break;

               // Success!
               case entity::Resource::ALLOCATE_OR_FREE_SUCCESS:

                  // Notify every entity in the room that the resource has
                  // been taken EXCEPT the one who's doing the taking
                  for (auto const &thing: location->getThings()) {
                     if (thing.get() != this) {
                        thing->out("notifications")
                           << getProperty<std::string>(TitleProperty)
                           << " takes " << resource->amountToString(amount) << ' '
                           << resource->titleToString(amount) << '.' << std::endl;
                     }
                  };

                  if (message.length() > 0) {
                     out("display") << message << std::endl;
                  } else {
                     out("display") << "You take "
                        << resource->amountToString(amount) << ' '
                        << resource->titleToString(amount) << '.' << std::endl;
                  }

                  if (doEvents) {
                     game->event({
                        "afterTakeResource",
                        {triggers.get(), resource->getEventListener()},
                        {this, resource.get(), amount}
                     });
                  }

                  break;

               // If we get ALLOCATE_OR_FREE_ABORT due to cancellation by the
               // event handler, we'll let the specific event trigger that
               // canceled the allocation handle the explanitory output.
               case entity::Resource::ALLOCATE_OR_FREE_ABORT:
               default:
                  break;
            }
         }
      }
   }

   /***************************************************************************/

   void Being::drop(
      const std::shared_ptr<Object> &object,
      bool checkUndroppable,
      bool doEvents
   ) {

      if (auto location = getLocation().lock()) {

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

            out("display") << "You can't drop that!" << std::endl;
            return;
         }

         removeFromInventory(object);
         location->insertThing(object);

         // Notify every entity in the room that the object has been dropped EXCEPT
         // the one who's doing the dropping
         for (auto const &thing: location->getThings()) {
            if (thing.get() != this) {
               thing->out("notifications")
                  << getProperty<std::string>(TitleProperty) << " drops "
                  << object->getProperty<std::string>(TitleProperty) << "."
                  << std::endl;
            }
         };

         std::string message = object->getMessage("drop");

         if (message.length() > 0) {
            out("display") << message << std::endl;
         } else {
            out("display") << "You drop the " << object->getName()
               << "." << std::endl;
         }

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

   // Events will be triggered by freeResource() and transferResourceToPlace()
   void Being::dropResource(
      const std::shared_ptr<Resource> &resource,
      double amount,
      bool doEvents
   ) {

      if (resource->isTagSet(entity::Resource::EphemeralTag)) {
         freeResource(resource.get(), amount, doEvents);
      } else {
         transferResourceToPlace(resource.get(), amount, doEvents);
      }
   }

   /***************************************************************************/

   int Being::calcDamage(Being *defender, Object *weapon) {

      int damage;

      damage = round(getProperty<int>(DamageBareHandsProperty) * getAttributeFactor("strength"));

      // make sure we always do at least 1 point damage
      damage = damage > 0 ? damage : 1;

      if (0 != weapon && weapon->isTagSet(Object::WeaponTag)) {
         damage += weapon->getProperty<int>(Object::DamageProperty);
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

      double defenderWoundRate = defender->getProperty<double>(WoundRateProperty);

      // probability that the attack will be successful
      double p = CLAMP(getAttributeFactor("strength") * (defenderWoundRate / 2) +
         (defenderWoundRate / 2), 0.0, defenderWoundRate);

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

         out("combat") << defender->getProperty<std::string>(TitleProperty)
            << " is already dead." << std::endl;
         return;
      }

      // if defender is immortal, then there's really no point...
      if (defender->isImmortal()) {

         if (!game->event({"attackDefenderIsImmortal", listeners, args})) {
            return;
         }

         out("combat") << defender->getProperty<std::string>(TitleProperty)
            << " is immortal and cannot die." << std::endl;
         return;
      }

      // Defender isn't attackable
      else if (!defender->isTagSet(Being::AttackableTag)) {

         if (!game->event({"attackDefenderNotAttackable", listeners, args})) {
            return;
         }

         out("combat") << defender->getProperty<std::string>(TitleProperty)
            << " cannot be attacked." << std::endl;
         return;
      }

      // send notification to the aggressor
      out("combat") << "You attack "
         << defender->getProperty<std::string>(TitleProperty);

      if (0 != weapon) {
         out("combat") << " with "
            << weapon->getProperty<std::string>(TitleProperty);
      }

      out("combat") << '.' << std::endl;

      // send notification to the defender
      defender->out("combat") << "You're attacked by "
         << getProperty<std::string>(TitleProperty);

      if (0 != weapon) {
         defender->out("combat") << " with "
            << weapon->getProperty<std::string>(TitleProperty);
      }

      defender->out("combat") << '!' << std::endl;

      if (isAttackSuccessful(defender)) {

         if (!game->event({"attackSuccess", listeners, args})) {
            return;
         }

         int damage = calcDamage(defender, weapon);

         defender->removeHealth(damage);

         out("combat") << "You dealt a blow to "
            << defender->getProperty<std::string>(TitleProperty) << "!"
               << std::endl;
         defender->out("combat") << getProperty<std::string>(TitleProperty)
            << " dealt you a blow!" << std::endl;
         out("combat") << defender->getProperty<std::string>(TitleProperty)
            << " loses " << damage << " health points." << std::endl;
         defender->out("combat") << "You lose " << damage << " health points."
            << std::endl;

         if (!defender->isAlive()) {
            return;
         }
      }

      else {

         if (!game->event({"attackFailure", listeners, args})) {
            return;
         }

         out("combat") << "Your attack failed." << std::endl;
         defender->out("combat") << getProperty<std::string>(TitleProperty)
            << "'s attack failed." << std::endl;
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
         location->out("notifications") << getProperty<std::string>(TitleProperty)
            << " dies." << std::endl;
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
