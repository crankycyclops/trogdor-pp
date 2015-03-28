#ifndef THREAD_H
#define THREAD_H


#if PTHREAD
#include <cstdio>
#include <pthread.h>
#else
#include <boost/thread/thread.hpp>
#endif

// Thread type
#if PTHREAD
typedef pthread_t thread_t;
#else
typedef boost::thread * thread_t;
#endif

// Mutex type
#if PTHREAD
typedef pthread_mutex_t mutex_t;
#else
typedef boost::mutex mutex_t;
#endif

// Thread creation
#if PTHREAD
#define THREAD_CREATE(THREAD, FUNC, ARG, ERRMSG) \
if (pthread_create(&(THREAD), 0, &(FUNC), (ARG))) { \
   game->err() << (ERRMSG); \
   exit(EXIT_FAILURE); \
}
#else
#define THREAD_CREATE(THREAD, FUNC, ARG, ERRMSG) THREAD = new boost::thread((FUNC), (ARG));
#endif

// Mutex init (do nothing for boost)
#if PTHREAD
#define INIT_MUTEX(X) pthread_mutex_init(&(X), 0)
#else
#define INIT_MUTEX(X)
#endif

// Mutex destroy (do nothing for boost)
#if PTHREAD
#define DESTROY_MUTEX(X) pthread_mutex_destroy(&(X))
#else
#define DESTROY_MUTEX(X)
#endif

// Mutex lock
#if PTHREAD
#define MUTEX_LOCK(X) pthread_mutex_lock(&(X))
#else
#define MUTEX_LOCK(X) (X).lock()
#endif

// Mutex unlock
#if PTHREAD
#define MUTEX_UNLOCK(X) pthread_mutex_unlock(&(X))
#else
#define MUTEX_UNLOCK(X) (X).unlock()
#endif


#endif

