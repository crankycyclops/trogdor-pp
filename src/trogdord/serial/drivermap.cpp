#include "../include/serial/drivermap.h"
#include "../include/exception/serverexception.h"

#ifdef ENABLE_SERIALIZE_JSON
	#include <trogdor/serial/drivers/json.h>
#endif

namespace serial {


	// Indicates that the singleton drivers have been instantiated.
	bool DriverMap::driversInstantiated = false;

	// Driver names mapped to singleton instances of trogdor::serial::Driver
	std::unordered_map<
		std::string,
		std::unique_ptr<trogdor::serial::Driver>
	> DriverMap::drivers;

	/************************************************************************/

	void DriverMap::instantiateDrivers() {

		#ifdef ENABLE_SERIALIZE_JSON
			drivers[trogdor::serial::Json::CLASS_NAME] = std::make_unique<trogdor::serial::Json>();
		#endif

		driversInstantiated = true;
	}

	/************************************************************************/

	std::unique_ptr<trogdor::serial::Driver> &DriverMap::get(std::string name) {

		if (!driversInstantiated) {
			instantiateDrivers();
		}

		if (drivers.end() == drivers.find(name)) {
			throw ServerException(name + " is not a valid serialization driver.");
		}

		return drivers[name];
	}
}
