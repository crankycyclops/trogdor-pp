#include <trogdor/entities/being.h>
#include <trogdor/timer/jobs/respawn.h>


namespace trogdor {


   const char *RespawnTimerJob::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   RespawnTimerJob::RespawnTimerJob(const serial::Serializable &data, Game *g): TimerJob(data, g) {

      deadGuy = g->getBeing(std::get<std::string>(*data.get("deadGuy"))).get();
   }

   /**************************************************************************/

   void RespawnTimerJob::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(RespawnTimerJob)),
         [] (std::any arg) -> std::shared_ptr<TimerJob> {

            // Invoke the deserialization constructor
            if (typeid(std::tuple<serial::Serializable, Game *>) == arg.type()) {
               auto args = std::any_cast<std::tuple<serial::Serializable, Game *> &>(arg);
               return std::make_shared<RespawnTimerJob>(std::get<0>(args), std::get<1>(args));
            }

            else {
               throw UndefinedException("Unsupported argument type in RespawnTimerJob instantiator");
            }
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
