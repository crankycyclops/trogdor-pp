#include "../../include/io/input/local.h"

namespace input {


	// String representation of the driver's name
	const char *Local::DRIVER_NAME = "local";

	/************************************************************************/

	bool Local::isSet(
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

	void Local::set(
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

	std::optional<std::string> Local::consume(
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
