#include <cstdio>
#include "../include/timer/timer.h"

using namespace std;

namespace core {


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

      sleep(1);

      // increment the current game time
      pthread_mutex_lock(&(game->timerMutex));
      time++;
      pthread_mutex_unlock(&(game->timerMutex));

      for (list<TimerJob *>::iterator i = queue.begin(); i != queue.end(); ++i) {

         if ((*i)->getExecutions() != 0) {

            if (time - (*i)->getInitTime() >= (*i)->getStartTime() &&
            (time - (*i)->getInitTime() - (*i)->getStartTime()) % (*i)->getInterval() == 0) {

               // run the job
               pthread_mutex_lock(&(game->timerMutex));
               (*i)->execute(**i);
               pthread_mutex_unlock(&(game->timerMutex));

               // decrement executions (unless it's -1, which means the job
               // should execute indefinitely)
               if ((*i)->getExecutions() > 0) {
                  (*i)->decExecutions();
               }
            }
         }

         // job is expired, so remove it
         else {
            pthread_mutex_lock(&(game->timerMutex));
            list<TimerJob *>::iterator iprev = i++;
            queue.remove(*iprev);
            pthread_mutex_unlock(&(game->timerMutex));
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

         pthread_mutex_lock(&(game->timerMutex));
         active = true;

         // start the timer thread
         if (pthread_create(&thread, 0, &doTick, this)) {
            game->err() << "Failed to start the timer!\n";
            exit(EXIT_FAILURE);
         }

         pthread_mutex_unlock(&(game->timerMutex));
      }
   }

/******************************************************************************/

   void Timer::stop() {

      if (active) {
         pthread_mutex_lock(&(game->timerMutex));
         active = false;
         pthread_mutex_unlock(&(game->timerMutex));
      }
   }

/******************************************************************************/

   void Timer::reset() {

      pthread_mutex_lock(&(game->timerMutex));
      time = 0;
      clearJobs();
      pthread_mutex_unlock(&(game->timerMutex));
   }

/******************************************************************************/

   void Timer::insertJob(TimerJob *job) {

      DoInsertJobArg *arg = new DoInsertJobArg();

      arg->game = game;
      arg->timer = this;
      arg->job = job;
      
      // insert job asynchronously to avoid deadlock when a function called by
      // a job inserts another job
      if (pthread_create(&thread, 0, &doInsertJob, arg)) {
         game->err() << "Failed to insert timer job!\n";
         exit(EXIT_FAILURE);
      }
      
   }

/******************************************************************************/

   void *doTick(void *timerObj) {

      while (((Timer *)timerObj)->active) {
         ((Timer *)timerObj)->tick();
      }
   }

/******************************************************************************/

   void *doInsertJob(void *arg) {

      Game *g = ((DoInsertJobArg *)arg)->game;
      Timer *t = ((DoInsertJobArg *)arg)->timer;
      TimerJob *j = ((DoInsertJobArg *)arg)->job;

      pthread_mutex_lock(&(g->timerMutex));
      j->initTime = t->time;
      t->queue.insert(t->queue.end(), j);
      pthread_mutex_unlock(&(g->timerMutex));

      delete (DoInsertJobArg *)arg;
   }
}

