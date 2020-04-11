#include "../../../include/io/input/listeners/redis.h"

namespace input {


	// String representation of the listener's name
	const char *Redis::LISTENER_NAME = "redis";

	/************************************************************************/

	bool Redis::isStarted() {

		return started;
	}

	/************************************************************************/

	void Redis::start() {

		// TODO
		started = true;
	}

	/************************************************************************/

	void Redis::stop() {

		// TODO
		started = false;
	}
}
