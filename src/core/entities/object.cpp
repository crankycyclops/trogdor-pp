#include "../include/game.h"
#include "../include/entities/object.h"


namespace trogdor { namespace entity {


   // Tag is set if the Object is a weapon
   const char *Object::WeaponTag = "weapon";

   // Tag is set if the Object is untakeable
   const char *Object::UntakeableTag = "untakeable";

   // Tag is set if the Object is undroppable
   const char *Object::UndroppableTag = "undroppable";

   /***************************************************************************/

   void Object::updateOwnerWeaponCache() {

      if (owner != nullptr && owner->isType(ENTITY_CREATURE)) {
         static_cast<Creature *>(owner)->clearWeaponCache();
      }
   }

   /***************************************************************************/

   void Object::addAlias(std::string alias) {

      Thing::addAlias(alias);

      if (owner) {
         owner->indexInventoryItemName(alias, this);
      }
   }

   /***************************************************************************/

   void Object::setTag(std::string tag) {

      Entity::setTag(tag);

      if (WeaponTag == tag) {
         updateOwnerWeaponCache();
      }
   }

   /***************************************************************************/

   void Object::removeTag(std::string tag) {

      if (WeaponTag == tag) {
         updateOwnerWeaponCache();
      }
   }
}}

