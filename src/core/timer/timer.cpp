#include "../include/timer/timer.h"

using namespace std;

namespace trogdor { namespace core {


   // For debugging purposes; allows us to print a TimerJob object
   ostream &operator<<(ostream &out, const TimerJob &j) {

      cout << "initTime: " << j.getInitTime() << endl;
      cout << "startTime: " << j.getStartTime() << endl;
      cout << "interval: " << j.getInterval() << endl;
      cout << "executions: " << j.getExecutions() << endl;

      return out;
   }

/******************************************************************************/

   Timer::Timer(Game *gameRef) {

      game = gameRef;
      active = false;
      time = 0;
   }

/******************************************************************************/

   Timer::~Timer() {

      clearJobs();
   }

/******************************************************************************/

   void Timer::tick() {

      // look into std::chrono::high_resolution_clock for more accurate timekeeping
      static std::chrono::milliseconds tickInterval(TICK_MILLISECONDS);
      std::this_thread::sleep_for(tickInterval);

      // increment the current game time
      MUTEX_LOCK(game->timerMutex);
      time++;
      MUTEX_UNLOCK(game->timerMutex);

      for (list<TimerJob *>::iterator i = queue.begin(); i != queue.end(); ++i) {

         if ((*i)->getExecutions() != 0) {

            if (time - (*i)->getInitTime() >= (*i)->getStartTime() &&
            (time - (*i)->getInitTime() - (*i)->getStartTime()) % (*i)->getInterval() == 0) {

               // run the job
               MUTEX_LOCK(game->timerMutex);
               (*i)->execute(**i);
               MUTEX_UNLOCK(game->timerMutex);

               // decrement executions (unless it's -1, which means the job
               // should execute indefinitely)
               if ((*i)->getExecutions() > 0) {
                  (*i)->decExecutions();
               }
            }
         }

         // job is expired, so remove it
         else {
            MUTEX_LOCK(game->timerMutex);
            list<TimerJob *>::iterator iprev = i++;
            queue.remove(*iprev);
            MUTEX_UNLOCK(game->timerMutex);
         }
      }
   }

/******************************************************************************/

   void Timer::clearJobs() {

      for (list<TimerJob *>::iterator i = queue.begin(); i != queue.end(); ++i) {
         delete *i;
      }
   }

/******************************************************************************/

   void Timer::start() {

      if (!active) {

         MUTEX_LOCK(game->timerMutex);
         active = true;

         THREAD_CREATE(jobThread, [](void *timerObj) {

            while (((Timer *)timerObj)->active) {
               ((Timer *)timerObj)->tick();
            }
         }, this, "Failed to start the timer!\n");

         MUTEX_UNLOCK(game->timerMutex);
      }
   }

/******************************************************************************/

   void Timer::stop() {

      if (active) {
         MUTEX_LOCK(game->timerMutex);
         active = false;
         MUTEX_UNLOCK(game->timerMutex);
      }
   }

/******************************************************************************/

   void Timer::reset() {

      MUTEX_LOCK(game->timerMutex);
      time = 0;
      clearJobs();
      MUTEX_UNLOCK(game->timerMutex);
   }

/******************************************************************************/

   void Timer::insertJob(TimerJob *job) {

      InsertJobArg *arg = new InsertJobArg();

      arg->game = game;
      arg->timer = this;
      arg->job = job;
      
      // insert job asynchronously to avoid deadlock when a function called by
      // one job inserts another job
      THREAD_CREATE(insertJobThread, [](void *arg) {

         Game *g = ((InsertJobArg *)arg)->game;
         Timer *t = ((InsertJobArg *)arg)->timer;
         TimerJob *j = ((InsertJobArg *)arg)->job;

         MUTEX_LOCK(g->timerMutex);
         j->initTime = t->time;
         t->queue.insert(t->queue.end(), j);
         MUTEX_UNLOCK(g->timerMutex);

         delete (InsertJobArg *)arg;
      }, arg, "Failed to insert timer job!\n");
   }

}}

