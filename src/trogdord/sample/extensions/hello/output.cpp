#include "output.h"

#include <trogdord/config.h>
#include <trogdord/exception/unsupportedoperation.h>

namespace output {


	// String representation of the driver's name
	const char *HelloOut::DRIVER_NAME = "hello";

	// Singleton instance of the HelloOut driver
	std::unique_ptr<HelloOut> HelloOut::instance = nullptr;

	/************************************************************************/

	/*
		Fill this in if you need the constructor to do anything.
	*/
	HelloOut::HelloOut() {}

	/************************************************************************/

	/*
		Likewise, if you need the destructor to do anything, fill this in.
	*/
	HelloOut::~HelloOut() {}

	/************************************************************************/

	std::unique_ptr<HelloOut> &HelloOut::get() {

		if (!instance) {
			instance = std::unique_ptr<HelloOut>(new HelloOut());
		}

		return instance;
	}

	/************************************************************************/

	const char *HelloOut::getName() {

		return DRIVER_NAME;
	}

	/************************************************************************/

	/*
		Returns the number of messages in an entity's channel's output buffer.

		This method only needs to be implemented if the output driver needs to
		support the {"method":"get","scope":"entity","action":"output"}
		request (if you examine trogdord's built-in output drivers, you'll see
		that Local supports this while Redis doesn't.) This lets us know how
		many outputted messages are on the stack, waiting to be retrieved.
	*/
	size_t HelloOut::size(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		throw UnsupportedOperation("hello output driver does not support this operation");
		return 0;
	}

	/************************************************************************/

	/*
		Pops the oldest message from an entity's channel's output buffer.

		Like Driver::size(), this only needs to be implemented if the output
		driver supports the {"method":"get","scope":"entity","action":"output"}
		request, meaning there's a stack of outputted messages to pop from.
	*/
	std::optional<Message> HelloOut::pop(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		throw UnsupportedOperation("hello output driver does not support this operation");
		return std::nullopt;
	}

	/************************************************************************/

	/*
		If the driver supports the
		{"method":"get","scope":"entity","action":"output"} request, this
		pushes a message onto an entity's channel's output buffer. If not,
		this method should immediately send the message to its destination.
	*/
	void HelloOut::push(
		size_t gameId,
		std::string entityName,
		std::string channel,
		Message message
	) {

		// This sample output driver will just send messages to trogdord's
		// internal logger. A real driver, of course, would do something a
		// little more interesting :) See the built-in Local and Redis drivers
		// for examples of real code in action.
		Config::get()->err(trogdor::Trogerr::INFO)
			<< "Hello, World extension (hello.so) logged a message: Game id "
			<< gameId << ", entity: " << entityName << ", channel: " << channel
			<< ", message: \"" << message.content << "\"" << std::endl;
	}
}
