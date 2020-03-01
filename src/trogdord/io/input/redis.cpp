#include "../../include/io/input/redis.h"

namespace input {


	// String representation of the driver's name
	const char *Redis::DRIVER_NAME = "redis";

	/************************************************************************/

	bool Redis::isSet(
		size_t gameId,
		std::string entityName
	) {

		// TODO
		return false;
	}

	/************************************************************************/

	void Redis::set(
		size_t gameId,
		std::string entityName,
		std::string input
	) {

		// TODO
	}

	/************************************************************************/

	std::optional<std::string> Redis::consume(
		size_t gameId,
		std::string entityName
	) {

		// TODO
		return std::nullopt;
	}
}
