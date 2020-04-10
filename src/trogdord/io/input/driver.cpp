#include "../../include/config.h"
#include "../../include/exception/serverexception.h"

#include "../../include/io/input/driver.h"

namespace input {


	// Singleton instance of input::Driver
	std::unique_ptr<Driver> Driver::instance = nullptr;

	/************************************************************************/

	Driver::Driver() {}

	/************************************************************************/

	std::unique_ptr<Driver> &Driver::get() {

		if (!instance) {
			instance = std::unique_ptr<Driver>(new Driver());
		}

		return instance;
	}

	/************************************************************************/

	bool Driver::isSet(
		size_t gameId,
		std::string entityName
	) {

		bufferMutex.lock();

		bool exists = bufferExists(gameId, entityName) &&
			inputBuffers[gameId][entityName].isSet ? true : false;

		bufferMutex.unlock();
		return exists;
	}

	/************************************************************************/

	void Driver::set(
		size_t gameId,
		std::string entityName,
		std::string input
	) {

		bufferMutex.lock();

		if (inputBuffers.end() == inputBuffers.find(gameId)) {
			inputBuffers[gameId] = {};
		}

		inputBuffers[gameId][entityName] = {true, input};
		bufferMutex.unlock();
	}

	/************************************************************************/

	std::optional<std::string> Driver::consume(
		size_t gameId,
		std::string entityName
	) {

		bufferMutex.lock();

		if (!bufferExists(gameId, entityName) || !inputBuffers[gameId][entityName].isSet) {
			bufferMutex.unlock();
			return std::nullopt;
		}

		else {

			std::string value = inputBuffers[gameId][entityName].buffer;

			inputBuffers[gameId][entityName].isSet = false;
			bufferMutex.unlock();

			return value;
		}
	}
}
