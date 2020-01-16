#include <memory>
#include <algorithm>

#include <trogdor/timer/timer.h>
#include <trogdor/timer/timerjob.h>

namespace trogdor {


   // For debugging purposes; allows us to print a TimerJob object
   std::ostream &operator<<(std::ostream &out, const TimerJob &j) {

      std::cout << "initTime: " << j.getInitTime() << std::endl;
      std::cout << "startTime: " << j.getStartTime() << std::endl;
      std::cout << "interval: " << j.getInterval() << std::endl;
      std::cout << "executions: " << j.getExecutions() << std::endl;

      return out;
   }

/******************************************************************************/

   Timer::Timer(Game *gameRef): jobThread(nullptr), insertJobThread(nullptr) {

      game = gameRef;
      active = false;
      time = 0;
   }

/******************************************************************************/

   Timer::~Timer() {

      stop();

      if (jobThread && jobThread->joinable()) {
         jobThread->join();
      }

      if (insertJobThread && insertJobThread->joinable()) {
         insertJobThread->join();
      }
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

      // I can't use for_each or a range-based for loop because I have to remove
      // expired jobs from queue and must be able to manipulate the iterator.
      for (std::list<std::shared_ptr<TimerJob>>::iterator i = queue.begin();
      i != queue.end(); ++i) {

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
            std::list<std::shared_ptr<TimerJob>>::iterator iprev = i++;
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

         jobThread = std::make_unique<std::thread>([](Timer *timerObj) {

            while ((timerObj)->active) {
               timerObj->tick();
            }
         }, this);

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
      insertJobThread = std::make_unique<std::thread>([](Game *g, Timer *t, std::shared_ptr<TimerJob> j) {

         g->timerMutex.lock();
         j->initTime = t->time;
         t->queue.insert(t->queue.end(), j);
         g->timerMutex.unlock();
      }, game, this, job);
   }
}
