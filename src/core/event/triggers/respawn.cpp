#include <memory>

#include "../../include/event/triggers/respawn.h"
#include "../../include/timer/jobs/respawn.h"

using namespace std;

namespace trogdor { namespace event {


   void RespawnEventTrigger::execute(EventArgumentList args) {

      Game  *game  = std::get<Game *>(args[0]);
      Being *being = static_cast<Being *>(std::get<Entity *>(args[1]));

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

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

