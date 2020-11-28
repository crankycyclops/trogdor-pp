#include <memory>

#include <trogdor/entities/being.h>

#include <trogdor/event/triggers/respawn.h>
#include <trogdor/timer/jobs/respawn.h>

namespace trogdor::event {


   EventReturn RespawnEventTrigger::operator()(Event e) {

      Game  *game  = std::get<Game *>(e.getArguments()[0]);
      entity::Being *being = static_cast<entity::Being *>(std::get<entity::Entity *>(e.getArguments()[1]));

      if (being->getProperty<bool>(entity::Being::RespawnEnabledProperty)) {

         int in = being->getProperty<int>(entity::Being::RespawnIntervalProperty);

         // we have to wait a certain number of clock ticks
         if (in > 0) {
            game->insertTimerJob(
               std::make_shared<RespawnTimerJob>(game, in, 1, in, being)
            );
         }

         // respawn right away
         else {
            being->doRespawn();
         }
      }

      return {true, true};
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> RespawnEventTrigger::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();

      data->set("type", "RespawnEventTrigger");
      return data;
   }
}
