#ifndef OUTPUT_DRIVER_H
#define OUTPUT_DRIVER_H

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>

#include "../message.h"

namespace output {


	class Driver {

		private:

			// Indicates that the singleton drivers have been instantiated.
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

			// Returns the number of messages in an entity's channel's output
			// buffer
			virtual size_t size(
				size_t gameId,
				std::string entityName,
				std::string channel
			) = 0;

			// Appends a new message to an entity's channel's output buffer
			virtual void push(
				size_t gameId,
				std::string entityName,
				std::string channel,
				OutputMessage message
			) = 0;

			// Pops the oldest message from an entity's channel's output
			// buffer. If the buffer is empty, std::nullopt is returned.
			virtual std::optional<OutputMessage> pop(
				size_t gameId,
				std::string entityName,
				std::string channel
			) = 0;
	};
}


#endif
