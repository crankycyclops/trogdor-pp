#ifndef TIMER_H
#define TIMER_H


#include <chrono>
#include <thread>
#include <mutex>

#include <list>
#include <iostream>
#include <cstdlib>

#include "../game.h"

// number of milliseconds in each clock tick
constexpr int TICK_MILLISECONDS = 1000;


using namespace std;

namespace trogdor { namespace core {


   class TimerJob; // full declaration occurs in timerjob.h

   /*
      The timer keeps track of a game's time and handles the scheduling of jobs
      that should be executed every n ticks of the clock.  It should be
      instantiated inside of a Game object, and maintains an internal reference
      to the game in which it was instantiated.
   */
   class Timer {

      private:

         Game *game;                    // the game in which the timer is running
         bool active;                   // whether or not the timer is active
         unsigned long time;            // current time
         list<TimerJob *> queue;        // queue of jobs to execute every n ticks

         /*
            Executes all jobs in the queue and increments the time.  This is
            called by the thread created in Timer::start() and shouldn't be
            called directly.

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
         inline bool isActive() const {return active;}

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

         /*
            Inserts a job into the queue for executions every n ticks of the
            clock.

            Input: Pointer to TimerJob object
            Output: (none)
         */
         void insertJob(TimerJob *job);

         /*
            Removes the job from the timer's work queue.  DO NOT call this from
            inside a TimerJob, as it will deadlock on timerMutex.  Instead, just
            set the TimerJob's number of executions to 0.  It will be reaped by
            the timer the next time it's encountered in the queue and will not
            be executed.

            Input: Pointer to TimerJob object to remove
            Output: (none)
         */
         inline void removeJob(TimerJob *job) {

            game->timerMutex.lock();
            queue.remove(job);
            game->timerMutex.unlock();
         }
   };
}}


#endif

