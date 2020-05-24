#include <random>
#include <trogdor/timer/jobs/wander.h>


using namespace trogdor::entity;

namespace trogdor {


   void WanderTimerJob::execute() {

      static std::random_device rd;
      static std::mt19937 generator(rd());
      static std::uniform_real_distribution<double> probabilityDist(0, 1);

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
      else if (probabilityDist(generator) > wanderer->getWanderLust()) {
         return;
      }

      Room *curLoc = static_cast<Room *>(wanderer->getLocation());
      unsigned int nConnections = curLoc->getNumConnections();

      // don't do anything if Creature is stuck in a room with no exits
      if (0 == nConnections) {
         return;
      }

      // TODO: use AI to select destination more intelligently; currently random
      // The whole rest of this function will change when we do...
      std::uniform_int_distribution<int> connectionsDist(0, nConnections - 1);
      wanderer->gotoLocation(curLoc->getConnectionByIndex(connectionsDist(generator)));

      // if wander interval ever changes, we should make sure it's updated
      setInterval(wanderer->getWanderInterval());
   }
}
