#ifndef RESPAWNTIMERJOB_H
#define RESPAWNTIMERJOB_H


#include <trogdor/timer/timerjob.h>
#include <trogdor/entities/being.h>


namespace trogdor {


   class RespawnTimerJob: public TimerJob {

      private:

         // "He's dead, Jim!" --Dr. McCoy
         std::weak_ptr<entity::Being> deadGuy;

      public:

         // The timer job's name. Used for type comparison.
         static constexpr const char *CLASS_NAME = "RespawnTimerJob";

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
            Constructor for the RespawnTimerJob class.
         */
         inline RespawnTimerJob(Game *g, int i, int e, int s,
         const std::shared_ptr<entity::Being> &b):
         TimerJob(g, i, e, s), deadGuy(b) {}

         /*
            Deserialization Constructor
         */
         RespawnTimerJob(const serial::Serializable &data, Game *g);

         /*
            Specifies who's doing the attacking (always a Creature.)

            Input:
               Pointer to attacker (Creature *)

            Output:
               (none)
         */
         inline void setDeadGuy(const std::shared_ptr<entity::Being> &b) {deadGuy = b;}

         /*
            Returns the instance's class name.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName();

         /*
            Executes Being's respawning.

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
