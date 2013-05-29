#include "../../include/timer/jobs/respawn.h"


using namespace std;

namespace core {


   void RespawnTimerJob::execute(TimerJob &job) {

      deadGuy->doRespawn();
   }
}

