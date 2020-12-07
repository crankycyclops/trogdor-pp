#include <trogdor/entities/being.h>
#include <trogdor/timer/jobs/respawn.h>


namespace trogdor {


   const char *RespawnTimerJob::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   void RespawnTimerJob::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(RespawnTimerJob)),
         [] (std::any arg) -> std::shared_ptr<TimerJob> {

            // TODO
         }
      );
   }

   /**************************************************************************/

   void RespawnTimerJob::execute() {

      deadGuy->doRespawn();
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> RespawnTimerJob::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>() = TimerJob::serialize();

      data->set("deadGuy", deadGuy->getName());
      return data;
   }
}
