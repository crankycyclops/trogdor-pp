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

			// Gets set to true once the singleton drivers have been
			// instantiated.
			static bool driversInstantiated;

			// Maps driver names to a singleton instance of that driver
			static std::unordered_map<std::string, std::unique_ptr<Driver>> drivers;

			// Instantiates the available output drivers (should only be
			// called once.)
			static void instantiateDrivers();

			// Used to keep track of the order in which messages should be
			// received. Each (game_id, entity name, channel) gets a separate
			// unsigned value that's incremented every time a message is
			// inserted into the output buffer. This ensures that clients who
			// receive messages over an unreliable network will be able to
			// put them in order before displaying them.
			std::unordered_map<
				size_t,
				std::unordered_map<
					std::string,
					std::unordered_map<
						std::string,
						size_t
					>
				>
			> messageOrder;

		protected:

			// Returns true if the driver has been activated and false if
			// not. Provides a trivial default implementation for drivers
			// that don't actually require any activation.
			virtual bool activated();

			// Any code that needs to be executed after instantiation and
			// before the driver can actually be used.
			virtual void activate();

			// Returns the current message order for the given game id,
			// entity name, and channel, then auto-increments the value.
			inline size_t nextOrder(
				size_t gameId,
				std::string entity,
				std::string channel
			) {

				if (messageOrder.end() == messageOrder.find(gameId)) {
					messageOrder[gameId] = {};
				}

				if (messageOrder[gameId].end() == messageOrder[gameId].find(entity)) {
					messageOrder[gameId][entity] = {};
				}

				if (messageOrder[gameId][entity].end() == messageOrder[gameId][entity].find(channel)) {
					messageOrder[gameId][entity][channel] = {};
				}

				size_t order = messageOrder[gameId][entity][channel];
				messageOrder[gameId][entity][channel]++;

				return order;
			}

		public:

			// Every driver should implement a destructor.
			virtual ~Driver() = 0;

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
				Message message
			) = 0;

			// Pops the oldest message from an entity's channel's output
			// buffer. If the buffer is empty, std::nullopt is returned.
			virtual std::optional<Message> pop(
				size_t gameId,
				std::string entityName,
				std::string channel
			) = 0;
	};
}


#endif
