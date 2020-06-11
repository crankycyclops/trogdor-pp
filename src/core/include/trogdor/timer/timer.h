#ifndef TIMER_H
#define TIMER_H


#include <chrono>
#include <thread>
#include <mutex>
#include <memory>

#include <list>
#include <iostream>
#include <cstdlib>

#include <trogdor/game.h>

// Number of milliseconds in each clock tick
constexpr int TICK_MILLISECONDS = 1000;

// How long the timer thread should sleep before checking to see if it needs to
// advance the clock
constexpr int THREAD_SLEEP_MILLISECONDS = 100;


namespace trogdor {


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

         std::mutex mutex;              // synchronize access to Timer

         // queue of jobs to execute every n ticks
         std::list<std::shared_ptr<TimerJob>> queue;

         // jobThread executes tick() and threads inside insertJobThreads allow
         // jobs to be inserted asynchronously
         std::unique_ptr<std::thread> jobThread;
         std::vector<std::unique_ptr<std::thread>> insertJobThreads;

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
         inline void clearJobs() {queue.clear();}

      public:

         /*
            Constructor for the Timer class.
         */
         Timer(Game *game);

         /*
            Destructor
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
            Deactivates the timer, but does not join the timer thread. Should be
            followed by a call to Timer::shutdown(). The only reason I provide
            this as a separate method and make it public is because it makes an
            optimization possible in the case where a lot of games need to be
            stopped at once. Under ordinary circumstances, you should only call
            start() and stop() directly.

            If you have to use this, it should always be followed by a call to
            shutdown, like so:

            timer->deactivate();
            ... any necessary cleanup code ...
            timer->shutdown();

            Input: (none)
            Output: (none)
         */
         void deactivate();
         /*

            Shuts down the timer thread after the timer has been deactivated.
            Should only be called after a call to deactivate(). The only reason
            I provide this as a separate method and make it public is because it
            makes an optimization possible in the case where a lot of games need
            to be stopped at once. Under ordinary circumstances, you should only
            call start() and stop() directly.

            If you have to use this, it should always be preceded by a call to
            deactivate, like so:

            timer->deactivate();
            ... any necessary cleanup code ...
            timer->shutdown();

            Input: (none)
            Output: (none)
         */
         void shutdown();

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
         void insertJob(std::shared_ptr<TimerJob> job);

         /*
            Removes the job from the timer's work queue.  DO NOT call this from
            inside a TimerJob, as it will deadlock on the mutex.  Instead, just
            set the TimerJob's number of executions to 0.  It will be reaped by
            the timer the next time it's encountered in the queue and will not
            be executed.

            Input: Pointer to TimerJob object we want to remove
            Output: (none)
         */
         inline void removeJob(std::shared_ptr<TimerJob> job) {

            mutex.lock();
            queue.remove(job);
            mutex.unlock();
         }
   };
}


#endif
