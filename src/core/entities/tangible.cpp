#include <trogdor/entities/tangible.h>
#include <trogdor/entities/being.h>


namespace trogdor::entity {


   Tangible::Tangible(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Entity(g, n, std::move(o), std::move(e)) {

      types.push_back(ENTITY_TANGIBLE);
   }

   /***************************************************************************/

   Tangible::Tangible(const Tangible &t, std::string n): Entity(t, n) {}

   /***************************************************************************/

   Tangible::Tangible(Game *g, const serial::Serializable &data): Entity(g, data) {

      // TODO
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Tangible::serialize() {

      std::shared_ptr<serial::Serializable> data = Entity::serialize();
      std::vector<std::string> serializedGlancedBy;

      for (const auto &glancedBy: glancedByMap) {
         if (auto observer = glancedBy.lock()) {
            serializedGlancedBy.push_back(observer->getName());
         }
      }

      data->set("glancedBy", serializedGlancedBy);

      std::vector<std::string> serializedObservedBy;

      for (const auto &observedBy: observedByMap) {
         if (auto observer = observedBy.lock()) {
            serializedObservedBy.push_back(observer->getName());
         }
      }

      data->set("observedBy", serializedObservedBy);

      std::vector<std::shared_ptr<serial::Serializable>> serializedResources;

      for (const auto &resource: resources) {

         if (auto resourcePtr = resource.first.lock()) {

            std::shared_ptr<serial::Serializable> serializedResource = std::make_shared<serial::Serializable>();

            serializedResource->set("resource", resourcePtr->getName());
            serializedResource->set("amount", resource.second);

            serializedResources.push_back(serializedResource);
         }
      }

      data->set("resources", serializedResources);
      return data;
   }

   /***************************************************************************/

   void Tangible::observe(const std::shared_ptr<Being> &observer, bool triggerEvents, bool displayFull) {

      if (triggerEvents && !game->event({
         "beforeObserve",
         {triggers.get(), observer->getEventListener()},
         {this, observer.get()}
      })) {
         return;
      }

      display(observer.get(), displayFull);

      mutex.lock();
      observedByMap.insert(observer);
      mutex.unlock();

      if (triggerEvents) {
         game->event({
            "afterObserve",
            {triggers.get(), observer->getEventListener()},
            {this, observer.get()}
         });
      }
   }

   /***************************************************************************/

   void Tangible::glance(const std::shared_ptr<Being> &observer, bool triggerEvents) {

      if (triggerEvents && !game->event({
         "beforeGlance",
         {triggers.get(), observer->getEventListener()},
         {this, observer.get()}
      })) {
         return;
      }

      displayShort(observer);

      mutex.lock();
      glancedByMap.insert(observer);
      mutex.unlock();

      if (triggerEvents) {
         game->event({
            "afterGlance",
            {triggers.get(), observer->getEventListener()},
            {this, observer.get()}
         });
      }
   }

   /***************************************************************************/

   void Tangible::display(Being *observer, bool displayFull) {

      std::shared_ptr<Being> observerShared = observer->getShared();

      if (!observedBy(observerShared) || displayFull) {
         if (ENTITY_PLAYER == observer->getType()) {
            observer->out("display") << (isPropertySet(LongDescProperty) ? getProperty<std::string>(LongDescProperty) : "")
               << std::endl;
         }
      }

      else {
         displayShort(observer);
      }
   }
}
