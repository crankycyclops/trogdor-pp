#ifndef OUTPUT_LOCAL_H
#define OUTPUT_LOCAL_H

#include <queue>
#include <string>
#include <mutex>
#include <unordered_map>

#include "driver.h"


namespace output {


	// Buffers output messages locally inside Trogdord.
	class Local: public Driver {

		private:

			// Make sure all operations on the local output buffer are thread-safe
			std::mutex bufferMutex;

			// Maps gameId -> entity name -> channel -> queue of messages.
			std::unordered_map<
				size_t,
				std::unordered_map<
					std::string,
					std::unordered_map<
						std::string,
						std::queue<Message>
					>
				>
			> buffer;

			// Returns true if a buffer has been initialized for the given
			// triple of gameId, entity name, and channel and false if not.
			inline bool isBufferInitialized(
				size_t gameId,
				std::string entityName,
				std::string channel
			) {

				if (buffer.end() == buffer.find(gameId)) {
					return false;
				}

				else if (buffer[gameId].end() == buffer[gameId].find(entityName)) {
					return false;
				}

				else if (buffer[gameId][entityName].end() == buffer[gameId][entityName].find(channel)) {
					return false;
				}

				else {
					return true;
				}
			}

		public:

			// Destructor
			virtual ~Local();

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
