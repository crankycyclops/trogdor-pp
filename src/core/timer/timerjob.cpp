#include <trogdor/timer/jobs/wander.h>
#include <trogdor/timer/jobs/respawn.h>
#include <trogdor/timer/jobs/autoattack.h>

namespace trogdor {


   std::unordered_map<std::string_view, std::type_info *> TimerJob::types;

   std::unordered_map<
      std::type_index,
      std::function<std::shared_ptr<TimerJob>(std::any)>
   > TimerJob::instantiators;

   /**************************************************************************/

   void TimerJob::registerBuiltinTypes() {

      WanderTimerJob::init();
      RespawnTimerJob::init();
      AutoAttackTimerJob::init();
   }

   /**************************************************************************/

   // See comment above destructor definition in include/timer/timerjob.h.
   TimerJob::~TimerJob() {}

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> TimerJob::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();

      data->set("type", getClassName());
      data->set("initTime", initTime);
      data->set("startTime", startTime);
      data->set("interval", interval);
      data->set("executions", executions);

      return data;
   }
}
