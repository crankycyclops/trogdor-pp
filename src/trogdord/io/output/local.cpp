#include "../../include/io/output/local.h"

namespace output {


	// String representation of the driver's name
	const char *Local::DRIVER_NAME = "local";

	/************************************************************************/

	size_t Local::size(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {
		return isBufferInitialized(gameId, entityName, channel) ?
			buffer[gameId][entityName][channel].size() : 0;
	}

	/************************************************************************/

	void Local::push(
		size_t gameId,
		std::string entityName,
		std::string channel,
		OutputMessage message
	) {

		if (buffer.end() == buffer.find(gameId)) {
			buffer[gameId] = {};
		}

		if (buffer[gameId].end() == buffer[gameId].find(entityName)) {
			buffer[gameId][entityName] = {};
		}

		if (buffer[gameId][entityName].end() == buffer[gameId][entityName].find(channel)) {
			buffer[gameId][entityName][channel] = {};
		}

		buffer[gameId][entityName][channel].push(message);
	}

	/************************************************************************/

	// Pops the oldest message from an entity's channel's output
	// buffer
	std::optional<OutputMessage> Local::pop(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		if (!isBufferInitialized(gameId, entityName, channel)) {
			return std::nullopt;
		}

		OutputMessage m = buffer[gameId][entityName][channel].front();

		buffer[gameId][entityName][channel].pop();
		return m;
	}
}
