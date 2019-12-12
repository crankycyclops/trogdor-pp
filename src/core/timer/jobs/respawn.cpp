#include "../../include/timer/jobs/respawn.h"


namespace trogdor {


   void RespawnTimerJob::execute() {

      deadGuy->doRespawn();
   }
}

