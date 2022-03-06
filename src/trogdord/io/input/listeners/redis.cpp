#include <trogdord/config.h>
#include <trogdord/io/input/listeners/redis.h>

namespace input {


	// String representation of the listener's name
	const char *Redis::LISTENER_NAME = "redis";

	/************************************************************************/

	Redis::Redis() {

		int ms = Config::get()->getInt(
			Config::CONFIG_KEY_REDIS_CONNECTION_TIMEOUT
		);

		int seconds = ms / 1000;
		int microseconds = (ms % 1000) * 1000;

		hostname = Config::get()->getString(Config::CONFIG_KEY_REDIS_HOST);
		username = Config::get()->getString(Config::CONFIG_KEY_REDIS_USERNAME);
		password = Config::get()->getString(Config::CONFIG_KEY_REDIS_PASSWORD);
		port = Config::get()->getUInt(Config::CONFIG_KEY_REDIS_PORT);
		redisChannel = Config::get()->getString(Config::CONFIG_KEY_REDIS_INPUT_CHANNEL);

		connectionTimeout = {seconds, microseconds};
		connectionRetryInterval = Config::get()->getInt(
			Config::CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL
		);
	}

	/************************************************************************/

	Redis::~Redis() {

		stop();
	}

	/************************************************************************/

	bool Redis::connect() {

		redis = redisConnectWithTimeout(hostname.c_str(), port, connectionTimeout);

		if (nullptr == redis || redis->err) {

			if (redis) {
				Config::get()->err() << "Redis input listener: " << redis->errstr << std::endl;
			} else {
				Config::get()->err() << "Redis input listener: can't allocate context" << std::endl;
			}

			return false;
		}

		// Authenticate if necessary
		std::string authStr;

		if (password.length()) {
			authStr = password;
		}

		if (username.length()) {
			authStr = username + ' ' + authStr;
		}

		if (authStr.length()) {

			auto reply = redisCommand(redis, "AUTH %s", authStr.c_str());

			if (nullptr == reply || static_cast<redisReply *>(reply)->type == REDIS_REPLY_ERROR) {
				Config::get()->err() << "output::Redis: " << redis->errstr << std::endl;
				return false;
			}
		}

		return true;
	}

	/************************************************************************/

	bool Redis::isStarted() {

		return running;
	}

	/************************************************************************/

	void Redis::start() {

		running = true;

		// Create worker thread that will consume the message queue and push
		// the contents out to redis.
		redisWorker = std::make_unique<std::thread>([&]() {

			// Stores replies from the redis context
			void *reply = nullptr;

			while (running) {

				redisMutex.lock();

				if (!isConnected()) {

					// If connection wasn't successful, wait for a little
					// while and then try again
					if (!connect()) {
						std::this_thread::sleep_for (
							std::chrono::milliseconds(connectionRetryInterval)
						);
						redisMutex.unlock();
						continue;
					}

					reply = redisCommand(redis, "SUBSCRIBE %s", redisChannel.c_str());
					freeReplyObject(reply);
					reply = nullptr;
				}

				// This little potentially dangerous gem is my way of
				// sidestepping the issue described further in my comment
				// in the stop() method. It's a compromise that gives me at
				// least a little bit more thread-safety. Temporarily
				// unlocking this mutex before a blocking call to
				// redisGetReply() and locking on the mutex during the
				// command in stop() has already prevented at least one
				// segmentation fault, so this is helping, even if it looks
				// stupid.
				redisMutex.unlock();
				int status = redisGetReply(redis, &reply);
				redisMutex.lock();

				if (REDIS_OK == status) {

					// For some reason I don't understand, hiredis wanted
					// the reply to be an opaque data type. As a result, I
					// have to cast this.
					if (
						reply &&
						REDIS_REPLY_ARRAY == static_cast<redisReply *>(reply)->type &&
						3 == static_cast<redisReply *>(reply)->elements
					) {
						dispatch(static_cast<redisReply *>(reply)->element[2]->str);
					}

					freeReplyObject(reply);
					reply = nullptr;
				}

				// If there's an error, close the connection so we know we
				// need to reconnect.
				else if (running) {
					Config::get()->err() << "Redis input listener: " << redis->errstr << std::endl;
					redisFree(redis);
					redis = nullptr;
				}

				redisMutex.unlock();
			}
		});
	}

	/************************************************************************/

	void Redis::stop() {

		if (running) {

			running = false;

			// This is only partially thread-safe (look for more details in
			// my comments in the thread implementation in start().) It's
			// error prone, but it's the only way I can get the blocking call
			// to redisGetReply() to return when no input is coming in. I
			// tried making the call non-blocking by calling redisSetTimeout(),
			// but then CPU utilization shoots up to 100%. With the hiredis
			// library, I just can't find a good way to do this short of
			// using the async functions (which I might have to do.) Another
			// idea I had was to try and close the socket with a call to
			// close(redis->fd), but that didn't work either. Bah!
			redisMutex.lock();
			redisCommand(redis, "QUIT");
			redisMutex.unlock();

			redisWorker->join();
		}
	}
}
