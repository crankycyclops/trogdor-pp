#include <memory>
#include <algorithm>

#include "../include/timer/timer.h"
#include "../include/timer/timerjob.h"

using namespace std;

namespace trogdor {


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

   void Timer::tick() {

      // look into std::chrono::high_resolution_clock for more accurate timekeeping
      static std::chrono::milliseconds tickInterval(TICK_MILLISECONDS);
      std::this_thread::sleep_for(tickInterval);

      // increment the current game time
      game->timerMutex.lock();
      time++;
      game->timerMutex.unlock();

      // I can't use for_each because I have to remove expired jobs from queue
      // and must be able to manipulate the iterator.
      for (list<std::shared_ptr<TimerJob>>::iterator i = queue.begin(); i != queue.end(); ++i) {

         if ((*i)->getExecutions() != 0) {

            if (time - (*i)->getInitTime() >= (*i)->getStartTime() &&
            (time - (*i)->getInitTime() - (*i)->getStartTime()) % (*i)->getInterval() == 0) {

               // run the job
               game->timerMutex.lock();
               (*i)->execute();
               game->timerMutex.unlock();

               // decrement executions (unless it's -1, which means the job
               // should execute indefinitely)
               if ((*i)->getExecutions() > 0) {
                  (*i)->decExecutions();
               }
            }
         }

         // job is expired, so remove it
         else {
            game->timerMutex.lock();
            list<std::shared_ptr<TimerJob>>::iterator iprev = i++;
            queue.remove(*iprev);
            game->timerMutex.unlock();
         }
      }
   }

/******************************************************************************/

   void Timer::start() {

      if (!active) {

         game->timerMutex.lock();
         active = true;

         std::thread jobThread([](Timer *timerObj) {

            while ((timerObj)->active) {
               timerObj->tick();
            }
         }, this);

         jobThread.detach();
         game->timerMutex.unlock();
      }
   }

/******************************************************************************/

   void Timer::stop() {

      if (active) {
         game->timerMutex.lock();
         active = false;
         game->timerMutex.unlock();
      }
   }

/******************************************************************************/

   void Timer::reset() {

      game->timerMutex.lock();
      time = 0;
      clearJobs();
      game->timerMutex.unlock();
   }

/******************************************************************************/

   void Timer::insertJob(std::shared_ptr<TimerJob> job) {

      // insert job asynchronously to avoid deadlock when a function called by
      // one job inserts another job
      std::thread insertJobThread([](Game *g, Timer *t, std::shared_ptr<TimerJob> j) {

         g->timerMutex.lock();
         j->initTime = t->time;
         t->queue.insert(t->queue.end(), j);
         g->timerMutex.unlock();
      }, game, this, job);

      insertJobThread.detach();
   }

}

