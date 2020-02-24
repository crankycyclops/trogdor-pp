#include <thread>
#include <chrono>

#include "../../include/config.h"
#include "../../include/io/input/driver.h"
#include "../../include/io/iostream/serverin.h"
#include "../../include/exception/serverexception.h"


trogdor::Trogin &ServerIn::operator>> (std::string &val) {

	// How long to sleep before polling for input again
	static std::chrono::milliseconds pollInterval(
		INPUT_POLL_INTERVAL
	);

	if (!entityPtr) {
		throw ServerException("Entity pointer not set on input stream before use. This is a bug.");
	}

	std::unique_ptr<input::Driver> &inBuffer = input::Driver::get(
		Config::get()->value<std::string>(Config::CONFIG_KEY_INPUT_DRIVER)
	);

	// Block until input is available
	while (!inBuffer->isSet(gameId, entityPtr->getName())) {
		std::this_thread::sleep_for(pollInterval);
	}

	val = *inBuffer->consume(gameId, entityPtr->getName());
	return *this;
}

/*****************************************************************************/

std::unique_ptr<trogdor::Trogin> ServerIn::clone() {

	std::unique_ptr<ServerIn> copy = std::make_unique<ServerIn>(gameId);

	copy->setEntity(entityPtr);
	return copy;
}
