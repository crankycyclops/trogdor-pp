#ifndef RESPAWNTIMERJOB_H
#define RESPAWNTIMERJOB_H


#include <trogdor/timer/timerjob.h>


namespace trogdor {


   class RespawnTimerJob: public TimerJob {

      private:

         // "He's dead, Jim!" --Dr. McCoy
         entity::Being *deadGuy;

      public:

         /*
            Constructor for the RespawnTimerJob class.
         */
         inline RespawnTimerJob(Game *g, int i, int e, int s, entity::Being *b):
         TimerJob(g, i, e, s), deadGuy(b) {}

         /*
            Specifies who's doing the attacking (always a Creature.)

            Input:
               Pointer to attacker (Creature *)

            Output:
               (none)
         */
         inline void setDeadGuy(entity::Being *b) {deadGuy = b;}

         /*
            Executes Being's respawning.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void execute();
   };
}


#endif
