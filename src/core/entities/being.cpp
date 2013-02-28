#include "../include/entities/being.h"
#include "../include/entities/object.h"


using namespace std;

namespace core { namespace entity {

   bool Being::insertIntoInventory(Object *object, bool considerWeight) {

      // make sure the Object will fit
      if (considerWeight &&
      inventory.currentWeight + object->getWeight() > inventory.weight) {
         return false;
      }

      // insert the object into the Being's inventory
      inventory.objects.insert(object);

      // allow referencing of inventory Objects by name and aliases
      for (int i = aliases.size() - 1; i >= 0; i--) {

         if (inventory.objectsByName.find(aliases[i]) == inventory.objectsByName.end()) {
            ObjectList newList;
            inventory.objectsByName[aliases[i]] = newList;
         }

         inventory.objectsByName.find(aliases[i])->second.push_back(object);
      }

      object->setOwner(this);
      object->setLocation(0);

      return true;
   }
}}
