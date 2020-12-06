#ifndef WANDERJOB_H
#define WANDERJOB_H


#include <trogdor/timer/timerjob.h>


namespace trogdor {


   class WanderTimerJob: public TimerJob {

      private:

         entity::Creature *wanderer;

      public:

         /*
            Constructor for the WanderTimerJob class.
         */
         inline WanderTimerJob(Game *g, int i, int e, int s, entity::Creature *c):
         TimerJob(g, i, e, s), wanderer(c) {}

         /*
            Executes Creature's wandering.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void execute();

         // Returns an easily serializable version of a TimerJob instance.
         virtual std::shared_ptr<serial::Serializable> serialize();
   };
}


#endif
