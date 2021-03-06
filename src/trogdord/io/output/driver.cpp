#include "../../include/config.h"
#include "../../include/exception/outputdrivernotfound.h"

#include "../../include/io/output/local.h"

#ifdef ENABLE_REDIS
	#include "../../include/io/output/redis.h"
#endif

namespace output {


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
			throw OutputDriverNotFound(name + " is not a valid output driver.");
		}

		if (!drivers[name]->activated()) {
			drivers[name]->activate();
		}

		return drivers[name];
	}

	/************************************************************************/

	// Default implementation for driver's that don't actually require
	// activation
	bool Driver::activated() {

		return true;
	}

	/************************************************************************/

	void Driver::activate() {}

	/************************************************************************/

	Driver::~Driver() {}
}
