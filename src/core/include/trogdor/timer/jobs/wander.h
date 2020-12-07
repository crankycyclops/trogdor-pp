#ifndef WANDERJOB_H
#define WANDERJOB_H


#include <trogdor/timer/timerjob.h>


namespace trogdor {


   class WanderTimerJob: public TimerJob {

      private:

         entity::Creature *wanderer;

      public:

         // The timer job's name. Used for type comparison.
         static constexpr const char *CLASS_NAME = "WanderTimerJob";

         /*
            Registers the timer job's type so that TimerJob knows how to copy
            and deserialize it later.

            Input:
               (none)

            Output:
               (none)
         */
         static void init();

         /*
            Constructor for the WanderTimerJob class.
         */
         inline WanderTimerJob(Game *g, int i, int e, int s, entity::Creature *c):
         TimerJob(g, i, e, s), wanderer(c) {}

         /*
            Returns the instance's class name.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName();

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
