#ifndef WANDERJOB_H
#define WANDERJOB_H


#include "../timer.h"


using namespace std;

namespace core {


   class WanderTimerJob: public TimerJob {

      private:

         Creature *wanderer;

      public:

         /*
            Constructor for the WanderTimerJob class.
         */
         inline WanderTimerJob(Game *g, int i, int e, int s, Creature *c):
         TimerJob(g, i, e, s) {

            wanderer = c;
         }

         /*
            Specifies who's doing the attacking (always a Creature.)

            Input:
               Pointer to wandering Creature (Creature *)

            Output:
               (none)
         */
         inline void setWanderer(Creature *c) {wanderer = c;}

         /*
            Executes Creature's wandering.

            Input:
               Reference to itself for possible modification during execution

            Output:
               (none)
         */
         virtual void execute(TimerJob &job);
   };
}


#endif
