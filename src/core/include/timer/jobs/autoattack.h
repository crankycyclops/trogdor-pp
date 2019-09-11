#ifndef AUTOATTACKTIMERJOB_H
#define AUTOATTACKTIMERJOB_H


#include "../timerjob.h"


using namespace std;

namespace trogdor { namespace core {


   class AutoAttackTimerJob: public TimerJob {

      private:

         Creature *aggressor;
         Being    *defender;

      public:

         /*
            Constructor for the AutoAttackTimerJob class.
         */
         inline AutoAttackTimerJob(Game *g, int i, int e, int s, Creature *a,
         Being *d): TimerJob(g, i, e, s) {

            aggressor = a;
            defender = d;
         }

         /*
            Specifies who's doing the attacking (always a Creature.)

            Input:
               Pointer to attacker (Creature *)

            Output:
               (none)
         */
         inline void setAttacker(Creature *a) {aggressor = a;}

         /*
            Specifies who's being attacked.

            Input:
               Pointer to defender (Being *)

            Output:
               (none)
         */
         inline void setDefender(Being *d) {defender = d;}

         /*
            Automatically executes an attack against a Being

            Input:
               (none)

            Output:
               (none)
         */
         virtual void execute();
   };
}}


#endif

