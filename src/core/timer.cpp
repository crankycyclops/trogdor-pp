#include "include/timer.h"

using namespace std;

namespace core {


   Timer::Timer(Game *gameRef) {

      game = gameRef;
      active = false;
      lastId = 0;
      time = 0;
   }


   Timer::~Timer() {

      clearJobs();
   }


   void Timer::tick() {

      sleep(1);
      // TODO
      time++;
   }


   void Timer::clearJobs() {

      for (list<TimerJob *>::iterator i = queue.begin(); i != queue.end(); ++i) {
         delete *i;
      }
   }


   void Timer::start() {

      if (!active) {

         pthread_mutex_lock(&(game->timerMutex));
         active = true;

         // start the timer thread
         if (pthread_create(&thread, 0, &doTick, this)) {
            *(game->trogerr) << "Failed to start the timer!\n";
            exit(EXIT_FAILURE);
         }

         pthread_mutex_unlock(&(game->timerMutex));
      }
   }


   void Timer::stop() {

      if (active) {
         pthread_mutex_lock(&(game->timerMutex));
         active = false;
         pthread_mutex_unlock(&(game->timerMutex));
      }
   }


   void Timer::reset() {

      pthread_mutex_lock(&(game->timerMutex));
      time = 0;
      clearJobs();
      pthread_mutex_unlock(&(game->timerMutex));
   }


   unsigned long Timer::insertJob(TimerJob *job) {


      job->id = lastId++;
      job->initTime = time;

      pthread_mutex_lock(&(game->timerMutex));
      queue.insert(queue.end(), job);
      pthread_mutex_unlock(&(game->timerMutex));

      return job->id;
   }


   bool Timer::removeJob(unsigned long id) {

      for (std::list<TimerJob *>::iterator i = queue.begin(); i != queue.end();
      ++i) {
         if ((*i)->getId() == id) {
            queue.remove(*i);
            return true;
         }
      }

      return false;
   }


   void *doTick(void *timerObj) {

      while (((Timer *)timerObj)->active) {
         ((Timer *)timerObj)->tick();
      }
   }
}

