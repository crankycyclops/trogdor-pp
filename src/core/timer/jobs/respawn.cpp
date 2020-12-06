#include <trogdor/entities/being.h>
#include <trogdor/timer/jobs/respawn.h>


namespace trogdor {


   void RespawnTimerJob::execute() {

      deadGuy->doRespawn();
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> RespawnTimerJob::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>() = TimerJob::serialize();

      data->set("type", "RespawnTimerJob");
      data->set("deadGuy", deadGuy->getName());

      return data;
   }
}
