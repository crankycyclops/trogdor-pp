#include "../../include/config.h"
#include "../../include/io/output/redis.h"
#include "../../include/exception/unsupportedoperation.h"

namespace output {


	// String representation of the driver's name
	const char *Redis::DRIVER_NAME = "redis";

	/************************************************************************/

	Redis::Redis() {

		int ms = Config::get()->value<int>(
			Config::CONFIG_KEY_REDIS_CONNECTION_TIMEOUT
		);

		int seconds = ms / 1000;
		int microseconds = (ms % 1000) * 1000;

		hostname = Config::get()->value<std::string>(Config::CONFIG_KEY_REDIS_HOST);
		port = Config::get()->value<int>(Config::CONFIG_KEY_REDIS_PORT);
		redisChannel = Config::get()->value<std::string>(Config::CONFIG_KEY_REDIS_OUTPUT_CHANNEL);

		connectionTimeout = {seconds, microseconds};
		connectionRetryInterval = Config::get()->value<int>(
			Config::CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL
		);
	}

	/************************************************************************/

	Redis::~Redis() {

		if (running) {
			running = false;
			msgsReady.notify_one();
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

				if (msgQueue.size()) {

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

		std::unique_ptr<rapidjson::Document> msgJSON;
		message.order = nextOrder(gameId, entityName, channel);

 		msgJSON->CopyFrom(message.toJSONObject(), msgJSON->GetAllocator());

		msgJSON->AddMember("game_id", gameId, msgJSON->GetAllocator());
		msgJSON->AddMember("entity", rapidjson::StringRef(entityName.c_str()), msgJSON->GetAllocator());
		msgJSON->AddMember("channel", rapidjson::StringRef(channel.c_str()), msgJSON->GetAllocator());

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
