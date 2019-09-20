#include "../../include/timer/jobs/respawn.h"


using namespace std;

namespace trogdor {


   void RespawnTimerJob::execute() {

      deadGuy->doRespawn();
   }
}

