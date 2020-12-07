#include <trogdor/entities/room.h>
#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/wander.h>


using namespace trogdor::entity;

namespace trogdor {


   const char *WanderTimerJob::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   WanderTimerJob::WanderTimerJob(const serial::Serializable &data, Game *g): TimerJob(data, g) {

      wanderer = g->getCreature(std::get<std::string>(*data.get("wanderer"))).get();
   }

   /**************************************************************************/

   void WanderTimerJob::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(WanderTimerJob)),
         [] (std::any arg) -> std::shared_ptr<TimerJob> {

            // Invoke the deserialization constructor
            if (typeid(std::tuple<serial::Serializable, Game *>) == arg.type()) {
               auto args = std::any_cast<std::tuple<serial::Serializable, Game *> &>(arg);
               return std::make_shared<WanderTimerJob>(std::get<0>(args), std::get<1>(args));
            }

            else {
               throw UndefinedException("Unsupported argument type in WanderTimerJob instantiator");
            }
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
