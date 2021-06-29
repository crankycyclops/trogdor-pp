#include <trogdord/config.h>
#include <trogdord/exception/outputdrivernotfound.h>

#include <trogdord/io/output/local.h>

#ifdef ENABLE_REDIS
	#include <trogdord/io/output/redis.h>
#endif

namespace output {


	// Indicates that the singleton drivers have been instantiated.
	bool Driver::driversInstantiated = false;

	// Driver names mapped to singleton instances of Driver
	std::unordered_map<std::string, Driver *> Driver::drivers;

	// Keeps track of which drivers are built-in. This is important, because
	// built-in drivers cannot be unregistered later, while drivers loaded by
	// extensions can.
	std::unordered_set<std::string> Driver::builtins;

	/************************************************************************/

	void Driver::instantiateDrivers() {

		drivers[Local::get()->getName()] = Local::get().get();
		builtins.insert(Local::get()->getName());

		#ifdef ENABLE_REDIS
			drivers[Redis::get()->getName()] = Redis::get().get();
			builtins.insert(Redis::get()->getName());
		#endif

		driversInstantiated = true;
	}

	/************************************************************************/

	Driver *Driver::get(std::string name) {

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
