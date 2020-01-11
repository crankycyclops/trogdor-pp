#ifndef WANDERJOB_H
#define WANDERJOB_H


#include <trogdor/timer/timerjob.h>


namespace trogdor {


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
            Executes Creature's wandering.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void execute();
   };
}


#endif
