#include <trogdor/game.h>

#include <trogdor/entities/object.h>
#include <trogdor/entities/being.h>


namespace trogdor::entity {


   void Object::setPropertyValidators() {

      setPropertyValidator(WeightProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});
      setPropertyValidator(DamageProperty, [&](PropertyValue v) -> int {return isPropertyValueInt(v);});
   }

   /***************************************************************************/

   Object::Object(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Thing(g, n, std::move(o),  std::move(e)),
   owner(std::weak_ptr<Being>()) {

      if (DEFAULT_IS_WEAPON) {
         setTag(WeaponTag);
      }

      setProperty(WeightProperty, DEFAULT_WEIGHT);
      setProperty(DamageProperty, DEFAULT_DAMAGE);

      setPropertyValidators();
      types.push_back(ENTITY_OBJECT);
      setClass("object");
   }

   /***************************************************************************/

   Object::Object(const Object &o, std::string n): Thing(o, n),
   owner(std::weak_ptr<Being>()) {}

   /***************************************************************************/

   Object::Object(Game *g, const serial::Serializable &data): Thing(g, data) {

      setPropertyValidators();
      types.push_back(ENTITY_OBJECT);
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Object::serialize() {

      std::shared_ptr<serial::Serializable> data = Thing::serialize();

      // I don't have to serialize the Object's owner because it will already be
      // taken into account by a Being's serialization of its inventory
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
