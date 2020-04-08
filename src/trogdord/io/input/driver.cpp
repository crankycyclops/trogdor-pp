#include "../../include/config.h"
#include "../../include/exception/serverexception.h"

#include "../../include/io/input/local.h"

#ifdef ENABLE_REDIS
	#include "../../include/io/input/redis.h"
#endif

namespace input {

	// Indicates that the singleton drivers have been instantiated.
	bool Driver::driversInstantiated = false;

	// Driver names mapped to singleton instances of Driver
	std::unordered_map<std::string, std::unique_ptr<Driver>> Driver::drivers;

	/************************************************************************/

	void Driver::instantiateDrivers() {

		drivers[Local::DRIVER_NAME] = std::make_unique<Local>();

		#ifdef ENABLE_REDIS
			drivers[Redis::DRIVER_NAME] = std::make_unique<Redis>();
		#endif

		driversInstantiated = true;
	}

	/************************************************************************/

	std::unique_ptr<Driver> &Driver::get(std::string name) {

		if (!driversInstantiated) {
			instantiateDrivers();
		}

		if (drivers.end() == drivers.find(name)) {
			throw ServerException(name + " is not a valid input driver.");
		}

		return drivers[name];
	}
}
