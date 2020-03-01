#ifndef OUTPUT_REDIS_H
#define OUTPUT_REDIS_H

#include "driver.h"


namespace output {


	// Buffers output messages externally through a Redis cache.
	class Redis: public Driver {

		public:

			// String representation of the driver's name.
			static const char *DRIVER_NAME;

			// Returns the number of messages in an entity's channel's output
			// buffer.
			virtual size_t size(
				size_t gameId,
				std::string entityName,
				std::string channel
			);

			// Appends a new message to an entity's channel's output buffer.
			virtual void push(
				size_t gameId,
				std::string entityName,
				std::string channel,
				Message message
			);

			// Pops the oldest message from an entity's channel's output
			// buffer. If the buffer is empty, std::nullopt is returned.
			virtual std::optional<Message> pop(
				size_t gameId,
				std::string entityName,
				std::string channel
			);
	};
}


#endif
