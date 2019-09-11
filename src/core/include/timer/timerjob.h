#ifndef TIMERJOB_H
#define TIMERJOB_H


#include <memory>
#include "timer.h"


using namespace std;

namespace trogdor { namespace core {

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
         virtual void execute() = 0;

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
         friend void Timer::insertJob(std::shared_ptr<TimerJob> job);
   };
}}


#endif

