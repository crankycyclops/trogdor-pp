#include <trogdor/game.h>
#include <trogdor/entities/object.h>


namespace trogdor::entity {


   // Tag is set if the Object is a weapon
   const char *Object::WeaponTag = "weapon";

   // Tag is set if the Object is untakeable
   const char *Object::UntakeableTag = "untakeable";

   // Tag is set if the Object is undroppable
   const char *Object::UndroppableTag = "undroppable";

   /***************************************************************************/

   Object::Object(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Thing(g, n, std::move(o),
   std::make_unique<NullIn>(), std::move(e)), owner(nullptr),
   weight(DEFAULT_WEIGHT), damage(DEFAULT_DAMAGE) {

      if (DEFAULT_IS_WEAPON) {
         setTag(WeaponTag);
      }

      types.push_back(ENTITY_OBJECT);
      setClass("object");
   }

   /***************************************************************************/

   Object::Object(const Object &o, std::string n): Thing(o, n) {

      owner = o.owner;
      weight = o.weight;
      damage = o.damage;
   }

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
}
