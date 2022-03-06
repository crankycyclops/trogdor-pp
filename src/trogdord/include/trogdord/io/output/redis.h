#ifndef OUTPUT_REDIS_H
#define OUTPUT_REDIS_H

#include <queue>
#include <thread>
#include <memory>
#include <condition_variable>

#include <hiredis.h>
#include <rapidjson/document.h>

#include "driver.h"


namespace output {


	// Buffers output messages externally through a Redis cache.
	class Redis: public Driver {

		private:

			// Singleton instance of the Redis output driver
			static std::unique_ptr<Redis> instance;

			// Whether or not the driver is considered activated. Setting
			// this to false after a thread is started to push messages to
			// redis results in that thread shutting down.
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

			// Messages waiting to be published to redis
			std::queue<std::unique_ptr<rapidjson::Document>> msgQueue;

			// Thread that will asynchronously publish messages to redis
			std::unique_ptr<std::thread> redisWorker;

			// Synchronizes access to msgQueue
			std::mutex redisMutex;

			// Notifies the redisWorker thread when there are messages to
			// publish
			std::condition_variable msgsReady;

			// Establishes a connection to the redis instance. Returns true
			// if successful and false if not.
			bool connect();

		protected:

			// Returns true once the driver's been activated
			virtual bool activated();

			// Setup a thread to asynchronously publish messages to redis
			virtual void activate();

		public:

			// String representation of the driver's name.
			static const char *DRIVER_NAME;

			// Returns singleton instance of the Redis output driver.
			static std::unique_ptr<Redis> &get();

			// Constructor
			Redis();
			Redis(const Redis &) = delete;

			// Destructor
			virtual ~Redis();

			// Returns the driver's name
			virtual const char *getName();

			// Returns whether or not we're currently connected to redis
			inline bool isConnected() {

				return redis && redis->flags & REDIS_CONNECTED ? true : false;
			}

			// Due to the ephemeral nature of Redis's PUBSUB model, This
			// method is unsupported and will throw UnsupportedOperation.
			virtual size_t size(
				size_t gameId,
				std::string entityName,
				std::string channel
			);

			// Appends a new message to an entity's channel's output buffer.
			virtual void push(
				size_t gameId,
				std::string entityName,
				std::string channel,
				Message message
			);

			// Due to the ephemeral nature of Redis's PUBSUB model, This
			// method is unsupported and will throw UnsupportedOperation.
			virtual std::optional<Message> pop(
				size_t gameId,
				std::string entityName,
				std::string channel
			);
	};
}


#endif
