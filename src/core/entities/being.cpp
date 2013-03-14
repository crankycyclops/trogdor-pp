#include "../include/entities/being.h"
#include "../include/entities/object.h"

#include "../include/game.h"


using namespace std;

namespace core { namespace entity {

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

      game->event("afterGotoLocation", eventArgs);
   }

   /***************************************************************************/

   void Being::take(Object *object) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(object);

      if (!game->event("beforeTake", eventArgs)) {
         return;
      }

      if (!object->getTakeable()) {
         game->event("takeUntakeable", eventArgs);
         throw TAKE_UNTAKEABLE;
      }

      if (!insertIntoInventory(object)) {
         game->event("takeTooHeavy", eventArgs);
         throw TAKE_TOO_HEAVY;
      }

      else {
         object->getLocation()->removeThing(object);
      }

      game->event("afterTake", eventArgs);
   }

   /***************************************************************************/

   void Being::drop(Object *object) {

      EventArgumentList eventArgs;

      eventArgs.push_back(this);
      eventArgs.push_back(object);

      if (!game->event("beforeDrop", eventArgs)) {
         return;
      }

      if (!object->getDroppable()) {
         game->event("dropUndroppable", eventArgs);
         throw DROP_UNDROPPABLE;
      }

      location->insertThing(object);
      removeFromInventory(object);

      game->event("afterDrop", eventArgs);
   }
}}

