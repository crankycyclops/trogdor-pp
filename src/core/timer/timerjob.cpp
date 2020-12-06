#include <trogdor/timer/timerjob.h>

namespace trogdor {


   // See comment above destructor definition in include/timer/timerjob.h.
   TimerJob::~TimerJob() {}

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> TimerJob::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();

      data->set("initTime", initTime);
      data->set("startTime", startTime);
      data->set("interval", interval);
      data->set("executions", executions);

      return data;
   }
}
