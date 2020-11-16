#include <trogdor/game.h>

#include <trogdor/entities/object.h>
#include <trogdor/entities/being.h>


namespace trogdor::entity {


   Object::Object(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Thing(g, n, std::move(o),  std::move(e)),
   owner(std::weak_ptr<Being>()) {

      if (DEFAULT_IS_WEAPON) {
         setTag(WeaponTag);
      }

      setProperty(WeightProperty, DEFAULT_WEIGHT);
      setProperty(DamageProperty, DEFAULT_DAMAGE);

      setPropertyValidator(WeightProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});
      setPropertyValidator(DamageProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});

      types.push_back(ENTITY_OBJECT);
      setClass("object");
   }

   /***************************************************************************/

   Object::Object(const Object &o, std::string n): Thing(o, n),
   owner(std::weak_ptr<Being>()) {}

   /***************************************************************************/

   Object::Object(const serial::Serializable &data): Thing(data) {

      // TODO
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Object::serialize() {

      std::shared_ptr<serial::Serializable> data = Thing::serialize();

      // TODO
      return data;
   }

   /***************************************************************************/

   void Object::addAlias(std::string alias) {

      Thing::addAlias(alias);
      std::shared_ptr<Being> ownerShared = owner.lock();

      if (ownerShared) {
         ownerShared->indexInventoryItemName(alias, this);
      }
   }
}
