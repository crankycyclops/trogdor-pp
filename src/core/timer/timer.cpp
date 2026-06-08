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

   Timer::Timer(Game *gameRef, std::optional<size_t> interval):
   game(gameRef), active(false), time(0), jobThread(nullptr), lastTickTime(0) {

      setTickInterval(interval ? *interval : TIMER_DEFAULT_TICK_MILLISECONDS);
   }

/******************************************************************************/

   Timer::Timer(Game *gameRef, const serial::Serializable &data): game(gameRef) {

      time = std::get<size_t>(*data.get("time"));
      tickInterval = std::chrono::milliseconds(std::get<size_t>(*data.get("tickInterval")));
      lastTickTime = std::chrono::milliseconds(std::get<size_t>(*data.get("lastTickTime")));
      jobThreadSleepTime = std::chrono::milliseconds(std::get<size_t>(*data.get("jobThreadSleepTime")));

      if (data.arraySize("jobs")) {

         std::vector<std::shared_ptr<serial::Serializable>> serializedJobs =
            std::get<std::vector<std::shared_ptr<serial::Serializable>>>(*data.get("jobs"));

         for (const auto &job: serializedJobs) {

            std::string typeName = std::get<std::string>(*job->get("type"));

            queue.insert(queue.end(), TimerJob::instantiate(
               typeName.c_str(),
               std::tuple<serial::Serializable, Game *>({*job, game})
            ));
         }
      }

      if (std::get<bool>(*data.get("active"))) {
         start();
      }
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

   void Timer::setTickInterval(size_t period) {

      std::lock_guard<Timer> lock(*this);

      tickInterval = std::chrono::milliseconds(period);
      jobThreadSleepTime = std::chrono::milliseconds(THREAD_SLEEP_MILLISECONDS);

      if (tickInterval < jobThreadSleepTime) {
         jobThreadSleepTime = tickInterval;
      }
   }

/******************************************************************************/

   std::shared_ptr<serial::Serializable> Timer::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();
      std::vector<std::shared_ptr<serial::Serializable>> serializedJobs;

      data->set("active", active);
      data->set("time", time);

      // Casting int64_t -> size_t is *probably* safe, but could fail on a
      // 32-bit system
      data->set("tickInterval", static_cast<size_t>(tickInterval.count()));
      data->set("lastTickTime", static_cast<size_t>(lastTickTime.count()));
      data->set("jobThreadSleepTime", static_cast<size_t>(jobThreadSleepTime.count()));

      // cppcheck wants me to use std::transform instead of the loop. Umm, no.
      for (const auto &job: queue) {
         serializedJobs.push_back(job->serialize());
      }

      data->set("jobs", serializedJobs);
      return data;
   }

/******************************************************************************/

   void Timer::tick() {

      // increment the current game time. The braces ensure that the lock
      // automatically releases after incrementing the timer.
      {
         std::lock_guard<Timer> lock(*this);
         time++;
      }

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

               // Jobs mutate game state, so we lock on Game's mutex. The lock
               // is released when the braces below go out of scope (when the
               // execute statement finishes.)
               {
                  std::lock_guard<Game> gameLock(*game);
                  (*i)->execute();
               }

               // decrement executions (unless it's -1, which means the job
               // should execute indefinitely)
               if ((*i)->getExecutions() > 0) {
                  (*i)->decExecutions();
               }
            }
         }

         // The job is expired, so remove it
         else {
            std::lock_guard<Timer> lock(*this);
            std::list<std::shared_ptr<TimerJob>>::iterator iprev = i++;
            queue.remove(*iprev);
         }
      }
   }

/******************************************************************************/

   void Timer::start() {

      if (!active) {

         std::lock_guard<Timer> lock(*this);
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
      }
   }

/******************************************************************************/

void Timer::deactivate() {

   std::lock_guard<Timer> lock(*this);
   active = false;
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

      std::lock_guard<Timer> lock(*this);
      time = 0;
      clearJobs();
   }

/******************************************************************************/

   void Timer::insertJob(std::shared_ptr<TimerJob> job) {

      // insert job asynchronously to avoid deadlock when a function called by
      // one job inserts another job
      insertJobThreads.push_back(

         std::make_unique<std::thread>([&](Game *g, Timer *t, std::shared_ptr<TimerJob> j) {

            std::lock_guard<Timer> lock(*t);
            j->initTime = t->time;
            t->queue.insert(t->queue.end(), j);
         }, game, this, job)
      );
   }
}
