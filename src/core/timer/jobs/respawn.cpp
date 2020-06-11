#include <trogdor/entities/being.h>
#include <trogdor/timer/jobs/respawn.h>


namespace trogdor {


   void RespawnTimerJob::execute() {

      deadGuy->doRespawn();
   }
}
