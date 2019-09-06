#ifndef TIMER_H
#define TIMER_H


#include <list>
#include <iostream>
#include <cstdlib>

#include <unistd.h>

#include "../thread.h"
#include "../game.h"

using namespace std;

namespace trogdor { namespace core {


   // Thread worker function that executes tick() inside of our timer object
   void *doTick(void *timerObj);

   // thread function that handles the insertion of a timer job so that
   // Timer::insertJob() can be asynchronous (avoids deadlocks when a job is
   // inserted somewhere inside of another job, which happens...)
   typedef struct {
      Game *game;
      Timer *timer;
      TimerJob *job;
   } DoInsertJobArg;

   void *doInsertJob(void *arg);

/******************************************************************************/

   class TimerJob; // full declaration occurs after Timer in this file

/******************************************************************************/

   /*
      The timer keeps track of a game's time and handles the scheduling of jobs
      that should be executed every n ticks of the clock.  It should be
      instantiated inside of a Game object, and maintains an internal reference
      to the game in which it was instantiated.
   */
   class Timer {

      private:

         Game *game;                // the game in which the timer is running
         bool active;               // whether or not the timer is active
         unsigned long time;        // current time
         list<TimerJob *> queue;    // queue of jobs to execute every n ticks

         thread_t jobThread;        // main timer thread (executes doTick)
         thread_t insertJobThread;  // thread that handles the insertion of a job

         /*
            Executes all jobs in the queue and increments the time.  This is
            called by the doTick() thread worker and shouldn't be called directly.

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

            MUTEX_LOCK(game->timerMutex);
            queue.remove(job);
            MUTEX_UNLOCK(game->timerMutex);
         }

         // thread function that bootstraps our timer object and calls tick()
         friend void *doTick(void *timerObj);

         // thread function that handles the insertion of a timer job so that
         // Timer::insertJob() can be asynchronous (avoids deadlocks when a job
         // is inserted somewhere inside of another job, which happens...)
         friend void *doInsertJob(void *arg);
   };

/******************************************************************************/

   /*
      TimerJob is an abstract class that represents a single job in a timer
      queue.  Each time the job is executed, execute() is called.  This class is
      meant to be inherited by a class written to accomplish a specific task.

      In a normal workflow, a typical use would be as follows (we'll assume that
      we're inside a method of Game that has direct access to the timer object
      and that there's a class, Task, which inherits TimerJob and performs some
      specific job):

      // NOTE: if inside a game object, you'll have to cast this to (Game *),
      // because this is a Game * const, not just Game *
      Task *task = new Task(currentGame, 1, 1, 1);

      The job has free reign to change any of its public or protected parameters
      at any time during execution.  For example, a job that was originally
      supposed to execute 1 time might increment executions to make it run one
      additional time.
   */
   class TimerJob {

      /*
         The following values are used by the job queue.
      */
      private:

         unsigned long initTime;   // time when the job was registered in the queue
         unsigned long startTime;  // how long to wait before job's first execution
         unsigned long interval;   // timer ticks between each job execution

         // Number of times to execute the job before removing it from the queue.
         // This value is decremented on every tick of the clock, and when it
         // reaches 0, it's removed from the queue. If set to -1, it will
         // continue to execute indefinitely until it's removed manually.
         int executions;

      protected:

         Game *game; // pointer to the game in which the timer resides

      public:

         /*
            Constructor for the TimerJob abstract class.
         */
         inline TimerJob(Game *g, int i, int e, int s) {

            initTime = 0; // will be set by insertJob()

            game = g;
            interval = i;
            executions = e;
            startTime = s;
         }

         /*
            This is the function that will be executed every time the job runs.
            It should be implemented in another class designed for a specific
            job.
         */
         virtual void execute(TimerJob &job) = 0;

         inline unsigned long getInitTime() const {return initTime;}
         inline unsigned long getStartTime() const {return startTime;}
         inline unsigned long getInterval() const {return interval;}
         inline int getExecutions() const {return executions;}

         inline void setStartTime(unsigned long s) {startTime = s;}
         inline void setInterval(unsigned long i) {interval = i;}
         inline void setExecutions(int e) {executions = e;}

         // decrement executions
         inline void decExecutions() {executions--;}

         // allows Timer to interact with the TimerJob object
         friend void Timer::insertJob(TimerJob *job);

         // allows us to insert a job asynchronously
         friend void *doInsertJob(void *arg);
   };
}}


#endif

