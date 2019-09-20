#include "../../include/dice.h"
#include "../../include/timer/jobs/wander.h"


using namespace std;
using namespace trogdor::entity;

namespace trogdor {


   void WanderTimerJob::execute() {

      Dice d;

      // make sure wandering wasn't turned off
      if (!wanderer->getWanderEnabled()) {
         setExecutions(0);
         return;
      }

      // make sure Creature isn't in some special non-Room Place with no
      // connections
      else if (ENTITY_ROOM != wanderer->getLocation()->getType()) {
         return;
      }

      // creature considers moving or staying; which will he pick?
      else if (d.get() > wanderer->getWanderLust()) {
         return;
      }

      // TODO: use AI to select destination more intelligently; currently random
      // The whole rest of this function will change when we do...
      d.roll();

      Room *curLoc = static_cast<Room *>(wanderer->getLocation());
      int nConnections = curLoc->getNumConnections();

      // don't do anything if Creature is stuck in a room with no exits
      if (0 == nConnections) {
         return;
      }

      int selection = d.getNormalized(nConnections);
      wanderer->gotoLocation(curLoc->getConnectionByIndex(selection));

      // if wander interval ever changes, we should make sure it's updated
      setInterval(wanderer->getWanderInterval());
   }
}

