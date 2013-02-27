#include "../include/entities/being.h"
#include "../include/entities/object.h"


using namespace std;

namespace core { namespace entity {

   bool Being::insertIntoInventory(Object *object, bool considerWeight) {
      // TODO
      // TODO: Make sure to update Object's owner field!
      cout << "STUB: Being::insertIntoInventory!" << endl;

      // make sure the Object will fit
      if (considerWeight &&
      inventory.currentWeight + object->getWeight() > inventory.weight) {
         return false;
      }

      object->setOwner(this);
      object->setLocation(0);
      return true;
   }
}}
