#include "../../include/config.h"
#include "../../include/io/output/driver.h"
#include "../../include/io/iostream/serverout.h"
#include "../../include/exception/serverexception.h"


void ServerOut::flush() {

	time_t curTime;
	time(&curTime);

	output::Message m = {
		curTime,
		getBufferStr()
	};

	// We haven't received a reference back to the entity that owns the stream
	// object yet, so queue the message until we know how to route it.
	if (!entityPtr) {
		msgBuffer.push(m);
	}

	else {

		std::unique_ptr<output::Driver> &outBuffer = output::Driver::get(
			Config::get()->getString(Config::CONFIG_KEY_OUTPUT_DRIVER)
		);

		outBuffer->push(gameId, entityPtr->getName(), getChannel(), m);
	}

	clear();
}

/*****************************************************************************/

void ServerOut::setEntity(trogdor::entity::Entity *e) {

	entityPtr = e;

	if (msgBuffer.size()) {

		std::unique_ptr<output::Driver> &outBuffer = output::Driver::get(
			Config::get()->getString(Config::CONFIG_KEY_OUTPUT_DRIVER)
		);

		while (msgBuffer.size()) {
			outBuffer->push(gameId, entityPtr->getName(), getChannel(), msgBuffer.front());
			msgBuffer.pop();
		}
	}
}

/*****************************************************************************/

std::unique_ptr<trogdor::Trogout> ServerOut::clone() {

	std::unique_ptr<ServerOut> stream = std::make_unique<ServerOut>(gameId);

	stream->setEntity(entityPtr);
	return stream;
}
