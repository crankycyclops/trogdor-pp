#include <memory>

#include <trogdor/event/triggers/respawn.h>
#include <trogdor/timer/jobs/respawn.h>

namespace trogdor::event {


   EventReturn RespawnEventTrigger::operator()(Event e) {

      Game  *game  = std::get<Game *>(e.getArguments()[0]);
      Being *being = static_cast<Being *>(std::get<Entity *>(e.getArguments()[1]));

      if (being->getRespawnEnabled()) {

         int in = being->getRespawnInterval();

         // we have to wait a certain number of clock ticks
         if (in > 0) {
            std::shared_ptr<RespawnTimerJob> j = std::make_shared<RespawnTimerJob>(game, in, 1, in, being);
            game->insertTimerJob(j);
         }

         // respawn right away
         else {
            being->doRespawn();
         }
      }

      return {true, true};
   }
}
