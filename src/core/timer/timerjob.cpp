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

   TimerJob::TimerJob(const serial::Serializable &data, Game *g): game(g) {

      initTime = data.getValue<size_t>("initTime");
      startTime = data.getValue<size_t>("startTime");
      interval = data.getValue<size_t>("interval");

      std::optional<serial::Value> e = data.get("executions");

      if (!e) {
         throw UndefinedException("Missing required value 'executions' during deserialization.");
      }

      if (e->index() == 0) {
         executions = std::get<size_t>(*e);
      } else {
         executions = std::get<int>(*e);
      }
   }

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
