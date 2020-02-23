#include "../../include/config.h"
#include "../../include/io/message.h"
#include "../../include/exception/serverexception.h"
#include "../../include/io/output/driver.h"
#include "../../include/io/iostream/serverout.h"


void ServerOut::flush() {

	if (!entityPtr) {
		throw ServerException("Entity pointer not set on output stream before use. This is a bug.");
	}

	output::Message m;

	time_t curTime;
	time(&curTime);

	std::unique_ptr<output::Driver> &outBuffer = output::Driver::get(
		Config::get()->value<std::string>(Config::CONFIG_KEY_OUTPUT_DRIVER)
	);

	outBuffer->push(gameId, entityPtr->getName(), getChannel(), m);
	clear();
}

/*****************************************************************************/

std::unique_ptr<trogdor::Trogout> ServerOut::clone() {

	std::unique_ptr<ServerOut> stream = std::make_unique<ServerOut>(gameId);

	stream->setEntity(entityPtr);
	return stream;
}
