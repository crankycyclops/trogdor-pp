#ifndef AUTOATTACKTIMERJOB_H
#define AUTOATTACKTIMERJOB_H


#include <trogdor/timer/timerjob.h>
#include <trogdor/entities/being.h>
#include <trogdor/entities/creature.h>


namespace trogdor {


   class AutoAttackTimerJob: public TimerJob {

      private:

         std::weak_ptr<entity::Creature> aggressor;
         std::weak_ptr<entity::Being>    defender;

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
         const std::shared_ptr<entity::Creature> &a,
         const std::shared_ptr<entity::Being> &d): TimerJob(g, i, e, s),
         aggressor(a), defender(d) {}

         /*
            Deserialization Constructor
         */
         AutoAttackTimerJob(const serial::Serializable &data, Game *g);

         /*
            Specifies who's doing the attacking (always a Creature.)

            Input:
               Attacker (std::shared_ptr<entity::Creature>)

            Output:
               (none)
         */
         inline void setAttacker(const std::shared_ptr<entity::Creature> &a) {aggressor = a;}

         /*
            Specifies who's being attacked.

            Input:
               Defender (std::shared_ptr<entity::Being>)

            Output:
               (none)
         */
         inline void setDefender(const std::shared_ptr<entity::Being> &d) {defender = d;}

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
