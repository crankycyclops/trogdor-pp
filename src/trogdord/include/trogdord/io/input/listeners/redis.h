#ifndef INPUT_LISTENER_REDIS_H
#define INPUT_LISTENER_REDIS_H

#include <thread>
#include <mutex>
#include <hiredis.h>

#include "listener.h"

namespace input {


	class Redis: public Listener {

		private:

			// Set to true if the listener has been started
			bool running = false;

			// Hiredis context containing state information about our
			// connection to redis
			redisContext *redis = nullptr;

			// How long we should attempt to connect before timing out
			timeval connectionTimeout;

			// If we fail to connect to redis, this is how long we should
			// wait before trying again
			int connectionRetryInterval;

			// Hostname we're connecting to
			std::string hostname;

			// Username used to authenticate (empty string if none)
			std::string username;

			// Password used to authenticate (empty string if none)
			std::string password;

			// The port redis is listening on
			int port;

			// The redis channel we should publish output messages to (not to
			// be confused with the trogdor-pp output channel)
			std::string redisChannel;

			// Synchronizes access to msgQueue
			std::mutex redisMutex;

			// Thread that will asynchronously publish messages to redis
			std::unique_ptr<std::thread> redisWorker;

			// Establishes a connection to the redis instance. Returns true
			// if successful and false if not.
			bool connect();

		public:

			// String representation of the listener's name.
			static const char *LISTENER_NAME;

			// Constructor
			Redis();

			// Destructor
			virtual ~Redis();

			// Returns whether or not we're currently connected to redis
			inline bool isConnected() {

				return redis && redis->flags & REDIS_CONNECTED ? true : false;
			}

			// Returns true if the listener has been started and false if not.
			virtual bool isStarted();

			// Starts the listener asynchronously and immediately returns.
			virtual void start();

			// Stops the listener. This method must block until all threads
			// associated with the listener have stopped. Not doing so will
			// result in undefined behavior.
			virtual void stop();
	};
}


#endif
