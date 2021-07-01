#include "mockoutputdriver.h"
#include <trogdord/exception/unsupportedoperation.h>

namespace output {


	// String representation of the driver's name
	const char *MockOutput::DRIVER_NAME = "mock";

	// Singleton instance of MockOutput
	std::unique_ptr<MockOutput> MockOutput::instance = nullptr;

	/************************************************************************/

	MockOutput::MockOutput() {}

	/************************************************************************/

	MockOutput::~MockOutput() {}

	/************************************************************************/

	std::unique_ptr<MockOutput> &MockOutput::get() {

		if (!instance) {
			instance = std::unique_ptr<MockOutput>(new MockOutput());
		}

		return instance;
	}

	/************************************************************************/

	const char *MockOutput::getName() {

		return DRIVER_NAME;
	}

	/************************************************************************/

	size_t MockOutput::size(size_t gameId, std::string entityName, std::string channel) {

		throw UnsupportedOperation("TODO");
		return 0;
	}

	/************************************************************************/

	void MockOutput::push(size_t gameId, std::string entityName, std::string channel, Message message) {

		throw UnsupportedOperation("TODO");
	}

	/************************************************************************/

	std::optional<Message> MockOutput::pop(size_t gameId, std::string entityName, std::string channel) {

		throw UnsupportedOperation("TODO");
		return std::nullopt;
	}
}