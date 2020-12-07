#include <trogdor/entities/room.h>
#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/wander.h>


using namespace trogdor::entity;

namespace trogdor {


   const char *WanderTimerJob::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   void WanderTimerJob::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(WanderTimerJob)),
         [] (std::any arg) -> std::shared_ptr<TimerJob> {

            // TODO
         }
      );
   }

   /**************************************************************************/

   void WanderTimerJob::execute() {

      if (!wanderer->getProperty<bool>(Creature::WanderEnabledProperty)) {
         setExecutions(0);
         return;
      }

      else {
         wanderer->wander();
      }

      // if wander interval ever changes, we should make sure it's updated
      setInterval(wanderer->getProperty<int>(Creature::WanderIntervalProperty));
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> WanderTimerJob::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>() = TimerJob::serialize();

      data->set("wanderer", wanderer->getName());
      return data;
   }
}
