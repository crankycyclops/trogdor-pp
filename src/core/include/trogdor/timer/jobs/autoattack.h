#ifndef AUTOATTACKTIMERJOB_H
#define AUTOATTACKTIMERJOB_H


#include <trogdor/timer/timerjob.h>


namespace trogdor {


   class AutoAttackTimerJob: public TimerJob {

      private:

         entity::Creature *aggressor;
         entity::Being    *defender;

      public:

         // The timer job's name. Used for type comparison.
         static constexpr const char *CLASS_NAME = "AutoAttackTimerJob";

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
            Constructor for the AutoAttackTimerJob class.
         */
         inline AutoAttackTimerJob(Game *g, int i, int e, int s,
         entity::Creature *a, entity::Being *d): TimerJob(g, i, e, s),
         aggressor(a), defender(d) {}

         /*
            Deserialization Constructor
         */
         AutoAttackTimerJob(const serial::Serializable &data, Game *g);

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
            Returns the instance's class name.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName();

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
