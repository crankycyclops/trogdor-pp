#ifndef AUTOATTACKTIMERJOB_H
#define AUTOATTACKTIMERJOB_H


#include <trogdor/timer/timerjob.h>


namespace trogdor {


   class AutoAttackTimerJob: public TimerJob {

      private:

         entity::Creature *aggressor;
         entity::Being    *defender;

      public:

         /*
            Constructor for the AutoAttackTimerJob class.
         */
         inline AutoAttackTimerJob(Game *g, int i, int e, int s,
         entity::Creature *a, entity::Being *d): TimerJob(g, i, e, s),
         aggressor(a), defender(d) {}

         /*
            Specifies who's doing the attacking (always a Creature.)

            Input:
               Pointer to attacker (Creature *)

            Output:
               (none)
         */
         inline void setAttacker(entity::Creature *a) {aggressor = a;}

         /*
            Specifies who's being attacked.

            Input:
               Pointer to defender (Being *)

            Output:
               (none)
         */
         inline void setDefender(entity::Being *d) {defender = d;}

         /*
            Automatically executes an attack against a Being

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
