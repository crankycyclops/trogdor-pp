#include "../../include/timer/jobs/respawn.h"


using namespace std;

namespace trogdor { namespace core {


   void RespawnTimerJob::execute(TimerJob &job) {

      deadGuy->doRespawn();
   }
}}

