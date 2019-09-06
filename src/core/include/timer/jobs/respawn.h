#ifndef RESPAWNTIMERJOB_H
#define RESPAWNTIMERJOB_H


#include "../timer.h"


using namespace std;

namespace trogdor { namespace core {


   class RespawnTimerJob: public TimerJob {

      private:

         // "He's dead, Jim!" --Dr. McCoy
         Being *deadGuy;

      public:

         /*
            Constructor for the RespawnTimerJob class.
         */
         inline RespawnTimerJob(Game *g, int i, int e, int s, Being *b):
         TimerJob(g, i, e, s) {

            deadGuy = b;
         }

         /*
            Specifies who's doing the attacking (always a Creature.)

            Input:
               Pointer to attacker (Creature *)

            Output:
               (none)
         */
         inline void setDeadGuy(Being *b) {deadGuy = b;}

         /*
            Executes Being's respawning.

            Input:
               Reference to itself for possible modification during execution

            Output:
               (none)
         */
         virtual void execute(TimerJob &job);
   };
}}


#endif

