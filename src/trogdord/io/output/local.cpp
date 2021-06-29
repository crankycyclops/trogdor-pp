#include <trogdord/io/output/local.h>

namespace output {


	// String representation of the driver's name
	const char *Local::DRIVER_NAME = "local";

	// Singleton instance of Local
	std::unique_ptr<Local> Local::instance = nullptr;

	/************************************************************************/

	std::unique_ptr<Local> &Local::get() {

		if (!instance) {
			instance = std::unique_ptr<Local>(new Local());
		}

		return instance;
	}

	/************************************************************************/

	const char *Local::getName() {

		return DRIVER_NAME;
	}

	/************************************************************************/

	size_t Local::size(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		bufferMutex.lock();

		size_t bufferSize = isBufferInitialized(gameId, entityName, channel) ?
			buffer[gameId][entityName][channel].size() : 0;

		bufferMutex.unlock();
		return bufferSize;
	}

	/************************************************************************/

	void Local::push(
		size_t gameId,
		std::string entityName,
		std::string channel,
		Message message
	) {

		bufferMutex.lock();

		if (buffer.end() == buffer.find(gameId)) {
			buffer[gameId] = {};
		}

		if (buffer[gameId].end() == buffer[gameId].find(entityName)) {
			buffer[gameId][entityName] = {};
		}

		if (buffer[gameId][entityName].end() == buffer[gameId][entityName].find(channel)) {
			buffer[gameId][entityName][channel] = {};
		}

		message.order = nextOrder(gameId, entityName, channel);
		buffer[gameId][entityName][channel].push(message);

		bufferMutex.unlock();
	}

	/************************************************************************/

	// Pops the oldest message from an entity's channel's output
	// buffer
	std::optional<Message> Local::pop(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		bufferMutex.lock();

		if (
			!isBufferInitialized(gameId, entityName, channel) ||
			!buffer[gameId][entityName][channel].size()
		) {
			bufferMutex.unlock();
			return std::nullopt;
		}

		Message m = buffer[gameId][entityName][channel].front();

		buffer[gameId][entityName][channel].pop();

		bufferMutex.unlock();
		return m;
	}

	/************************************************************************/

	Local::~Local() {}
}
