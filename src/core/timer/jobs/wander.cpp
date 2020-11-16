#include <trogdor/entities/room.h>
#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/wander.h>


using namespace trogdor::entity;

namespace trogdor {


   void WanderTimerJob::execute() {

      if (!wanderer->getProperty<bool>(Creature::WanderEnabledProperty)) {
         setExecutions(0);
         return;
      }

      else {
         wanderer->wander();
      }

      // if wander interval ever changes, we should make sure it's updated
      setInterval(wanderer->getProperty<int>(Creature::WanderIntervalProperty));
   }
}
