#ifndef MOCK_OUTPUT_H
#define MOCK_OUTPUT_H

#include <trogdord/io/output/driver.h>


namespace output {


	class MockOutput: public Driver {

		private:

			// Singleton instance of the Local output driver
			static std::unique_ptr<MockOutput> instance;

			// Constructor
			MockOutput();
			MockOutput(const MockOutput &copy) = delete;

		public:

			// String representation of the driver's name.
			static const char *DRIVER_NAME;

			// Returns singleton instance of the Local output driver.
			static std::unique_ptr<MockOutput> &get();

			// Destructor
			virtual ~MockOutput();

			// Returns the driver's name
			virtual const char *getName();

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
