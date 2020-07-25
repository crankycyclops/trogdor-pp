#include <trogdor/game.h>

#include <trogdor/entities/object.h>
#include <trogdor/entities/being.h>


namespace trogdor::entity {


   // Tag is set if the Object is a weapon
   const char *Object::WeaponTag = "weapon";

   // Tag is set if the Object is untakeable
   const char *Object::UntakeableTag = "untakeable";

   // Tag is set if the Object is undroppable
   const char *Object::UndroppableTag = "undroppable";

   /***************************************************************************/

   Object::Object(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Thing(g, n, std::move(o),  std::move(e)),
   weight(DEFAULT_WEIGHT), damage(DEFAULT_DAMAGE), owner(std::weak_ptr<Being>()) {

      if (DEFAULT_IS_WEAPON) {
         setTag(WeaponTag);
      }

      types.push_back(ENTITY_OBJECT);
      setClass("object");
   }

   /***************************************************************************/

   Object::Object(const Object &o, std::string n): Thing(o, n),
   weight(o.weight), damage(o.damage), owner(std::weak_ptr<Being>()) {}

   /***************************************************************************/

   void Object::addAlias(std::string alias) {

      Thing::addAlias(alias);
      std::shared_ptr<Being> ownerShared = owner.lock();

      if (ownerShared) {
         ownerShared->indexInventoryItemName(alias, this);
      }
   }
}
