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

   Timer::Timer(Game *gameRef, size_t interval):
   game(gameRef), active(false), time(0), jobThread(nullptr),
   tickInterval(interval), lastTickTime(0) {

      jobThreadSleepTime =
         std::chrono::milliseconds(THREAD_SLEEP_MILLISECONDS) > tickInterval ?
         tickInterval : std::chrono::milliseconds(THREAD_SLEEP_MILLISECONDS);
   }

/******************************************************************************/

   Timer::~Timer() {

      stop();

      // Join any remaining job insert threads to avoid any joinable threads
      // falling out of scope without being joined and causing whatever
      // application is linked against this library to prematurely abort.
      if (insertJobThreads.size()) {
         for (auto &insertJobThread: insertJobThreads) {
            insertJobThread->join();
         }
      }
   }

/******************************************************************************/

   void Timer::tick() {

      // increment the current game time
      mutex.lock();
      time++;
      mutex.unlock();

      // I can't use for_each or a range-based for loop because I have to remove
      // expired jobs from queue and must be able to manipulate the iterator.
      // Note: the cppcheck warning about the extra iterator increment can be
      // ignored, as we'll never have a situation where i gets incremented
      // beyond queue.end().
      for (std::list<std::shared_ptr<TimerJob>>::iterator i = queue.begin();
      i != queue.end(); ++i) {

         if ((*i)->getExecutions() != 0) {

            if (time - (*i)->getInitTime() >= (*i)->getStartTime() &&
            (time - (*i)->getInitTime() - (*i)->getStartTime()) % (*i)->getInterval() == 0) {

               // run the job
               mutex.lock();
               (*i)->execute();
               mutex.unlock();

               // decrement executions (unless it's -1, which means the job
               // should execute indefinitely)
               if ((*i)->getExecutions() > 0) {
                  (*i)->decExecutions();
               }
            }
         }

         // job is expired, so remove it
         else {
            mutex.lock();
            std::list<std::shared_ptr<TimerJob>>::iterator iprev = i++;
            queue.remove(*iprev);
            mutex.unlock();
         }
      }
   }

/******************************************************************************/

   void Timer::start() {

      if (!active) {

         mutex.lock();
         active = true;

         lastTickTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
         );

         jobThread = std::make_unique<std::thread>([&]() {

            while (active) {

               std::chrono::milliseconds curTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch()
               );

               if (curTime - lastTickTime >= tickInterval) {
                  tick();
                  lastTickTime = curTime;
               }

               std::this_thread::sleep_for(jobThreadSleepTime);
            }
         });

         mutex.unlock();
      }
   }

/******************************************************************************/

void Timer::deactivate() {

   mutex.lock();
   active = false;
   mutex.unlock();
}

/******************************************************************************/

void Timer::shutdown() {

   if (jobThread && jobThread->joinable()) {
      jobThread->join();
      jobThread = nullptr;
   }
}

/******************************************************************************/

   void Timer::stop() {

      if (active) {
         deactivate();
         shutdown();
      }
   }

/******************************************************************************/

   void Timer::reset() {

      mutex.lock();
      time = 0;
      clearJobs();
      mutex.unlock();
   }

/******************************************************************************/

   void Timer::insertJob(std::shared_ptr<TimerJob> job) {

      // insert job asynchronously to avoid deadlock when a function called by
      // one job inserts another job
      insertJobThreads.push_back(

         std::make_unique<std::thread>([&](Game *g, Timer *t, std::shared_ptr<TimerJob> j) {

            mutex.lock();
            j->initTime = t->time;
            t->queue.insert(t->queue.end(), j);
            mutex.unlock();
         }, game, this, job)
      );
   }
}
