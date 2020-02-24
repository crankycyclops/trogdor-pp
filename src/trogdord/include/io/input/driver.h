#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>

namespace input {


	class Driver {

		private:

			// Gets set to true once the singleton drivers have been
			// instantiated.
			static bool driversInstantiated;

			// Maps driver names to a singleton instance of that driver
			static std::unordered_map<std::string, std::unique_ptr<Driver>> drivers;

			// Instantiates the available output drivers (should only be
			// called once.)
			static void instantiateDrivers();

		public:

			// Returns the singleton instance of Driver matching the given name
			// (throws ServerException if the specified driver name hasn't
			// been mapped to an instance of Driver.)
			static std::unique_ptr<Driver> &get(std::string name);

			// Returns true if an entity's input buffer contains a string and
			// false if not.
			virtual bool isSet(
				size_t gameId,
				std::string entityName
			) = 0;

			// Sets the contents of an entity's input buffer. If called more
			// than once without calling get(), the buffer will be overwritten.
			virtual void set(
				size_t gameId,
				std::string entityName,
				std::string input
			) = 0;

			// Pops the contents of an entity's input buffer. If nothing is in
			// the buffer when this method is called, nothing will be returned.
			virtual std::optional<std::string> consume(
				size_t gameId,
				std::string entityName
			) = 0;
	};
}


#endif
