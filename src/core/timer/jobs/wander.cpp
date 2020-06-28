#include <trogdor/entities/room.h>
#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/wander.h>


using namespace trogdor::entity;

namespace trogdor {


   void WanderTimerJob::execute() {

      if (!wanderer->getWanderEnabled()) {
         setExecutions(0);
         return;
      }

      else {
         wanderer->wander();
      }

      // if wander interval ever changes, we should make sure it's updated
      setInterval(wanderer->getWanderInterval());
   }
}
