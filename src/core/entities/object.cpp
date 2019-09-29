#include "../include/game.h"
#include "../include/entities/object.h"


using namespace std;

namespace trogdor { namespace entity {


   void Object::updateOwnerWeaponCache() {

      if (owner != nullptr && owner->isType(ENTITY_CREATURE)) {
         static_cast<Creature *>(owner)->clearWeaponCache();
      }
   }

   /***************************************************************************/

   void Object::addAlias(string alias) {

      Thing::addAlias(alias);

      if (owner) {
         owner->indexInventoryItemName(alias, this);
      }
   }

   /***************************************************************************/

   void Object::setTag(string tag) {

      Entity::setTag(tag);

      if ("weapon" == tag) {
         updateOwnerWeaponCache();
      }
   }

   /***************************************************************************/

   void Object::removeTag(string tag) {

      if ("weapon" == tag) {
         updateOwnerWeaponCache();
      }
   }
}}

