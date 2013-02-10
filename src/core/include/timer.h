#ifndef TIMER_H
#define TIMER_H


#include <list>
#include <iostream>
#include <cstdlib>

#include <unistd.h>
#include <pthread.h>

#include "game.h"

using namespace std;

namespace core {


   // Thread function that executes tick() inside of our timer object
   void *doTick(void *timerObj);


   class TimerJob {

   };


   class Timer {

      private:

         Game *game;             // the game in which the timer is running
         bool active;            // whether or not the timer is active
         unsigned long lastId;   // last assigned job id
         unsigned long time;     // current time
         list<TimerJob *> queue; // queue of jobs to execute every n ticks
         pthread_t thread;       // thread id for timer thread

         /*
            Executes all jobs in the queue and increments the time.  This is
            called by pthread and shouldn't be called directly.

            Input: (none -- we don't really use it...)
            Output: (none)
         */
         void tick();

         /*
            Clears all jobs in the queue.  The current state of the clock itself
            is not affected.

            Input: (none)
            Output: (none)
         */
         void clearJobs();

      public:

         /*
            Constructor for the Timer class.
         */
         Timer(Game *game);

         /*
            Destructor for the Timer class.
         */
         ~Timer();

         /*
            Returns the current time.

            Input: (none)
            Output: Current time (unsigned long)
         */
         inline unsigned long getTime() const {return time;}

         /*
            Returns bool to determine whether or not the timer is ticking.

            Input: (none)
            Output: Whether or not timer is active (bool)
         */
         inline unsigned long isActive() const {return active;}

         /*
            Starts the timer.

            Input: (none)
            Output: (none)
         */
         void start();

         /*
            Stops the timer (but doesn't reset the current time.)

            Input: (none)
            Output: (none)
         */
         void stop();

         /*
            Clears the job queue and resets the current time to 0.  If the timer
            is still running when reset is called, the timer will continue to
            run, starting at 0.

            Input: (none)
            Output: (none)
         */
         void reset();

         // thread function that bootstraps our timer object and calls tick()
         friend void *doTick(void *timerObj);
   };
}


#endif

