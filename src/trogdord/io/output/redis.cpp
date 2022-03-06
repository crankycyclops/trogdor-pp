#include <trogdord/config.h>
#include <trogdord/io/output/redis.h>
#include <trogdord/exception/unsupportedoperation.h>

namespace output {


	// String representation of the driver's name
	const char *Redis::DRIVER_NAME = "redis";

	// Singleton instance of Local
	std::unique_ptr<Redis> Redis::instance = nullptr;

	/************************************************************************/

	std::unique_ptr<Redis> &Redis::get() {

		if (!instance) {
			instance = std::unique_ptr<Redis>(new Redis());
		}

		return instance;
	}

	/************************************************************************/

	const char *Redis::getName() {

		return DRIVER_NAME;
	}

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
		redisChannel = Config::get()->getString(Config::CONFIG_KEY_REDIS_OUTPUT_CHANNEL);

		connectionTimeout = {seconds, microseconds};
		connectionRetryInterval = Config::get()->getInt(
			Config::CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL
		);
	}

	/************************************************************************/

	Redis::~Redis() {

		if (running) {
			running = false;
			msgsReady.notify_all();
			redisWorker->join();
		}
	}

	/************************************************************************/

	bool Redis::connect() {

		redis = redisConnectWithTimeout(hostname.c_str(), port, connectionTimeout);

		if (nullptr == redis || redis->err) {

			if (redis) {
				Config::get()->err() << "output::Redis: " << redis->errstr << std::endl;
			} else {
				Config::get()->err() << "output::Redis: can't allocate context" << std::endl;
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

	bool Redis::activated() {

		return running;
	}

	/************************************************************************/

	void Redis::activate() {

		running = true;

		// Create worker thread that will consume the message queue and push
		// the contents out to redis.
		redisWorker = std::make_unique<std::thread>([&]() {

			while (running) {

				std::unique_lock<std::mutex> lock(redisMutex);

				msgsReady.wait_for(lock, std::chrono::milliseconds(connectionRetryInterval), [&]() {
					return (msgQueue.size() && (!redis || isConnected())) || !running;
				});

				if (running && msgQueue.size()) {

					// If we can't connect, wait for a little while and try
					// again
					if (!isConnected()) {

						if (!connect()) {
							lock.unlock();
							continue;
						}
					}

					while (msgQueue.size()) {

						auto reply = redisCommand(
							redis,
							"PUBLISH %s %s",
							redisChannel.c_str(),
							JSON::serialize(*msgQueue.front()).c_str()
						);

						// an error occurred
						if (!reply) {
							Config::get()->err() << "output::Redis: " <<
								redis->errstr << std::endl;
							break;
						}

						else {
							msgQueue.pop();
						}
					}
				}

				lock.unlock();
			}
		});
	}

	/************************************************************************/

	size_t Redis::size(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		throw UnsupportedOperation("redis output driver does not support this operation");
		return 0;
	}

	/************************************************************************/

	void Redis::push(
		size_t gameId,
		std::string entityName,
		std::string channel,
		Message message
	) {

		std::unique_ptr<rapidjson::Document> msgJSON = std::make_unique<rapidjson::Document>();
		message.order = nextOrder(gameId, entityName, channel);

		rapidjson::Value entityVal(rapidjson::StringRef(entityName.c_str()), msgJSON->GetAllocator());
		rapidjson::Value channelVal(rapidjson::StringRef(channel.c_str()), msgJSON->GetAllocator());

 		msgJSON->CopyFrom(message.toJSONObject(msgJSON->GetAllocator()), msgJSON->GetAllocator());
		msgJSON->AddMember("game_id", gameId, msgJSON->GetAllocator());
		msgJSON->AddMember("entity", entityVal.Move(), msgJSON->GetAllocator());
		msgJSON->AddMember("channel", channelVal.Move(), msgJSON->GetAllocator());

		std::unique_lock<std::mutex> lock(redisMutex);
		msgQueue.push(std::move(msgJSON));
		lock.unlock();

		msgsReady.notify_one();
	}

	/************************************************************************/

	// Pops the oldest message from an entity's channel's output
	// buffer
	std::optional<Message> Redis::pop(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		throw UnsupportedOperation("redis output driver does not support this operation");
		return std::nullopt;
	}
}
