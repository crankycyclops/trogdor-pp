#ifndef HELLO_OUTPUT_H
#define HELLO_OUTPUT_H

#include <queue>
#include <string>
#include <mutex>
#include <optional>
#include <unordered_map>

#include <trogdord/io/output/driver.h>

namespace output {


	class HelloOut: public Driver {

		private:

			// Singleton instance of the HelloOut driver
			static std::unique_ptr<HelloOut> instance;

		protected:

			// Construction should only occur through get()
			HelloOut();
			HelloOut(const HelloOut &) = delete;

		public:

			// String representation of the driver's name.
			static const char *DRIVER_NAME;

			// Returns singleton instance of the HelloOut output driver.
			static std::unique_ptr<HelloOut> &get();

			// Destructor
			virtual ~HelloOut();

			// Returns the driver's name
			virtual const char *getName();

			/*
				Returns the number of messages in an entity's channel's output
				buffer.

				This method only needs to be implemented if the output driver
				needs to support the {"method":"get","scope":"entity",
				"action":"output"} request (if you examine trogdord's built-in
				output drivers, you'll see that Local supports this while
				Redis doesn't.)
			*/
			virtual size_t size(
				size_t gameId,
				std::string entityName,
				std::string channel
			);

			/*
				If the driver supports the
				{"method":"get","scope":"entity","action":"output"} request,
				this pushes a message onto an entity's channel's output buffer.
				If not, this method immediately flushes the message.
			*/
			virtual void push(
				size_t gameId,
				std::string entityName,
				std::string channel,
				Message message
			);

			/*
				Pops the oldest message from an entity's channel's output
				buffer.

				Like Driver::size(), this only needs to be implemented if the
				output driver supports the {"method":"get","scope":"entity",
				"action":"output"} request.
			*/
			virtual std::optional<Message> pop(
				size_t gameId,
				std::string entityName,
				std::string channel
			);
	};
}


#endif
