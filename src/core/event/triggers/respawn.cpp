#include "../../include/event/triggers/respawn.h"
#include "../../include/timer/jobs/respawn.h"

using namespace std;

namespace trogdor { namespace core { namespace event {


   void RespawnEventTrigger::execute(EventArgumentList args) {

      Game  *game  = boost::get<Game *>(args[0]);
      Being *being = static_cast<Being *>(boost::get<Entity *>(args[1]));

      if (being->getRespawnEnabled()) {

         int in = being->getRespawnInterval();

         // we have to wait a certain number of clock ticks
         if (in > 0) {
            RespawnTimerJob *j = new RespawnTimerJob(game, in, 1, in, being);
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
}}}

