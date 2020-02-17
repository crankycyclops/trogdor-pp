#include "../../include/io/output/redis.h"

namespace output {


	// String representation of the driver's name
	const char *Redis::DRIVER_NAME = "redis";

	/************************************************************************/

	size_t Redis::size(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {
		// TODO
		return 0;
	}

	/************************************************************************/

	void Redis::push(
		size_t gameId,
		std::string entityName,
		std::string channel,
		OutputMessage message
	) {
		// TODO
		return;
	}

	/************************************************************************/

	// Pops the oldest message from an entity's channel's output
	// buffer
	std::optional<OutputMessage> Redis::pop(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		// TODO
		return std::nullopt;
	}
}
