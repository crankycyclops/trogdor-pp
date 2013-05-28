#include "../../include/timer/jobs/respawn.h"


using namespace std;

namespace core {


   void RespawnTimerJob::execute(TimerJob &job) {

      if (!deadGuy->isAlive()) {

         deadGuy->respawn();

         // it's possible that a respawn event prevented the Being from coming
         // back to life, so make sure it did before we send out the message
         if (deadGuy->isAlive()) {

            string msg = deadGuy->getMessage("respawn");

            if (0 == msg.length()) {
               msg = deadGuy->getName() + " comes back to life.";
            }

            *game->trogout << endl << msg << endl;
         }
      }
   }
}

