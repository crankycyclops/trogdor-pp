#include <cmath>

#include "../include/entities/being.h"
#include "../include/entities/object.h"

#include "../include/game.h"


using namespace std;

namespace core { namespace entity {


   void Being::display(Being *observer, bool displayFull) {

      if (ENTITY_PLAYER == observer->getType()) {

         if (!alive) {

            string descDead = msgs.get("description_dead");

            if (descDead.length() > 0) {
               *game->trogout << descDead << endl;
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

            *game->trogout << "You see the corpse of " << getTitle() << '.';

            string descDead = msgs.get("descshort_dead");

            if (descDead.length() > 0) {
               *game->trogout << ' ' << descDead;
            }

            *game->trogout << endl;
         }

         else {
            Thing::displayShort(observer);
         }
      }
   }

   /***************************************************************************/

   LuaTable *Being::getLuaTable() const {

      LuaTable *table = Thing::getLuaTable();

      table->setField("health", health);
      table->setField("maxhealth", maxHealth);
      table->setField("alive", alive);

      table->setField("attackable", attackable);
      table->setField("woundrate", woundRate);
      table->setField("damagebarehands", damageBareHands);

      LuaTable *attrs = new LuaTable();
      attrs->setField("strengthraw", attributes.strength);
      attrs->setField("dexterityraw", attributes.dexterity);
      attrs->setField("intelligenceraw", attributes.intelligence);
      attrs->setField("strength", getStrengthFactor());
      attrs->setField("dexterity", getDexterityFactor());
      attrs->setField("intelligence", getIntelligenceFactor());

      table->setField("attributes", *attrs);

      LuaTable *inv = new LuaTable();

      inv->setField("maxweight", inventory.weight);
      inv->setField("weight", inventory.currentWeight);
      inv->setField("itemcount", (int)inventory.count);

      int ipos = 0;
      stringstream s;
      LuaTable *invItems = new LuaTable();

      for (ObjectSet::iterator i = inventory.objects.begin();
      i != inventory.objects.end(); i++) {
         s.str(std::string()), s << ipos++;
         invItems->setField(s.str(), (*i)->getName());
      }

      inv->setField("items", *invItems);
      table->setField("inventory", *inv);

      return table;
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
      vector<string> objAliases = object->getAliases();
      for (int i = objAliases.size() - 1; i >= 0; i--) {

         if (inventory.objectsByName.find(objAliases[i]) == inventory.objectsByName.end()) {
            ObjectList newList;
            inventory.objectsByName[objAliases[i]] = newList;
         }

         inventory.objectsByName.find(objAliases[i])->second.push_back(object);
      }

      inventory.count++;
      object->setOwner(this);
      return true;
   }

   /***************************************************************************/

   void Being::removeFromInventory(Object *object) {

      inventory.objects.erase(object);

      vector<string> objAliases = object->getAliases();
      for (int i = objAliases.size() - 1; i >= 0; i--) {
         inventory.objectsByName.find(objAliases[i])->second.remove(object);
      }

      inventory.count--;
      inventory.currentWeight -= object->getWeight();
      object->setOwner(0);
   }

   /***************************************************************************/

   void Being::gotoLocation(Place *l) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(location);
      eventArgs.push_back(l);

      game->addEventListener(l->getEventListener());
      game->addEventListener(triggers);
      if (!game->event("beforeGotoLocation", eventArgs)) {
         return;
      }

      setLocation(l);
      l->observe(this);

      game->addEventListener(l->getEventListener());
      game->addEventListener(triggers);
      game->event("afterGotoLocation", eventArgs);
   }

   /***************************************************************************/

   void Being::take(Object *object) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(object);

      game->addEventListener(triggers);
      game->addEventListener(object->getEventListener());
      if (!game->event("beforeTake", eventArgs)) {
         return;
      }

      if (!object->getTakeable()) {
         game->addEventListener(triggers);
         game->addEventListener(object->getEventListener());
         game->event("takeUntakeable", eventArgs);
         throw TAKE_UNTAKEABLE;
      }

      if (!insertIntoInventory(object)) {
         game->addEventListener(triggers);
         game->addEventListener(object->getEventListener());
         game->event("takeTooHeavy", eventArgs);
         throw TAKE_TOO_HEAVY;
      }

      else {
         object->getLocation()->removeThing(object);
      }

      game->addEventListener(triggers);
      game->addEventListener(object->getEventListener());
      game->event("afterTake", eventArgs);
   }

   /***************************************************************************/

   void Being::drop(Object *object) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(object);

      game->addEventListener(triggers);
      game->addEventListener(object->getEventListener());
      if (!game->event("beforeDrop", eventArgs)) {
         return;
      }

      if (!object->getDroppable()) {
         game->addEventListener(triggers);
         game->addEventListener(object->getEventListener());
         game->event("dropUndroppable", eventArgs);
         throw DROP_UNDROPPABLE;
      }

      location->insertThing(object);
      removeFromInventory(object);

      game->addEventListener(triggers);
      game->addEventListener(object->getEventListener());
      game->event("afterDrop", eventArgs);
   }

   /***************************************************************************/

   int Being::calcDamage(Being *defender, Object *weapon) {

      int damage;

      damage = round(damageBareHands * getStrengthFactor());

      // make sure we always do at least 1 point damage
      damage = damage > 0 ? damage : 1;

      if (0 != weapon && weapon->isWeapon()) {
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

   void Being::attack(Being *defender, Object *weapon, bool counterAttack) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(defender);
      eventArgs.push_back(weapon);

      game->addEventListener(triggers);
      game->addEventListener(defender->getEventListener());
      if (0 != weapon) {
         game->addEventListener(weapon->getEventListener());
      }

      if (!game->event("beforeAttack", eventArgs)) {
         return;
      }

      if (!isAlive()) {

         game->addEventListener(triggers);
         game->addEventListener(defender->getEventListener());
         if (0 != weapon) {
            game->addEventListener(weapon->getEventListener());
         }

         if (!game->event("attackPlayerAlreadyDead", eventArgs)) {
            return;
         }

         *game->trogout << getTitle() << " is already dead and cannot fight."
            << endl;
         return;
      }

      if (!defender->isAlive()) {

         game->addEventListener(triggers);
         game->addEventListener(defender->getEventListener());
         if (0 != weapon) {
            game->addEventListener(weapon->getEventListener());
         }

         if (!game->event("attackDefenderAlreadyDead", eventArgs)) {
            return;
         }

         *game->trogout << defender->getTitle() << " is already dead." << endl;
         return;
      }

      // used either if the attack is successful or if it fails
      game->addEventListener(triggers);
      game->addEventListener(defender->getEventListener());
      if (0 != weapon) {
         game->addEventListener(weapon->getEventListener());
      }

      if (isAttackSuccessful(defender)) {

         if (!game->event("attackSuccess", eventArgs)) {
            return;
         }

         int damage = calcDamage(defender, weapon);

         defender->removeHealth(damage);

         *game->trogout << getTitle() << " dealt a blow to "
            << defender->getTitle() << "!" << endl;
         *game->trogout << defender->getTitle() << " loses " << damage <<
            " health points." << endl;

         if (!defender->isAlive()) {
            *game->trogout << defender->getTitle() << " dies." << endl;
            return;
         }
      }

      else {

         if (!game->event("attackFailure", eventArgs)) {
            return;
         }

         *game->trogout << "Attack failed!" << endl;
      }

      if (counterAttack) {

         if (ENTITY_PLAYER != defender->getType()) {
            *game->trogout << defender->getTitle() << " fights back." << endl;
            // TODO: how will defender's weapon selection occur...?  Argh...
            defender->attack(this, 0, false);
         }
      }

      game->addEventListener(triggers);
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

      game->addEventListener(triggers);
      if (!game->event("beforeAddHealth", eventArgs)) {
         return;
      }

      health += up;

      if (!allowOverflow && health > maxHealth) {
         health = maxHealth;
      }

      game->addEventListener(triggers);
      game->event("afterAddHealth", eventArgs);
   }

   /***************************************************************************/

   void Being::removeHealth(int down, bool allowDeath) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(health);
      eventArgs.push_back(down);

      game->addEventListener(triggers);
      if (!game->event("beforeRemoveHealth", eventArgs)) {
         return;
      }

      health -= down;

      if (health <= 0) {

         health = 0; // just in case it's a negative number, we clip at 0

         if (allowDeath) {
            die();
         }
      }


      game->addEventListener(triggers);
      game->event("afterRemoveHealth", eventArgs);
   }

   /***************************************************************************/

   void Being::die() {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);

      game->addEventListener(triggers);
      if (!game->event("beforeDie", eventArgs)) {
         return;
      }

      alive = false;

      game->addEventListener(triggers);
      game->event("afterDie", eventArgs);
   }
}}
