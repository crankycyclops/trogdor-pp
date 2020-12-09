#ifndef OUTPUT_DRIVER_H
#define OUTPUT_DRIVER_H

#include <trogdor/serial/driver.h>

namespace serial {


	class DriverMap {

		private:

			// Gets set to true once the singleton drivers have been
			// instantiated.
			static bool driversInstantiated;

			// Maps driver names to a singleton instance of that driver
			static std::unordered_map<
				std::string,
				std::unique_ptr<trogdor::serial::Driver>
			> drivers;

			// Instantiates the available output drivers (should only be
			// called once.)
			static void instantiateDrivers();

		public:

			// Returns the singleton instance of a serialization driver
			// matching the given name (throws ServerException if the
			// specified driver name hasn't been mapped to an instance of
			// trogdor::serial::Driver.)
			static std::unique_ptr<trogdor::serial::Driver> &get(std::string name);
	};
}


#endif
