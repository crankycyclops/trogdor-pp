#include "streamout.h"


void PHPStreamOut::flush() {

	if (!entity) {
		zend_throw_exception(EXCEPTION_GLOBALS(validationException), "Entity pointer must be set before the PHPStreamOut instance can be used.", 0);
		return;
	}

	const std::string &entityName = entity->getName();

	if (gameData->outBuffer.end() == gameData->outBuffer.find(entityName)) {
		gameData->outBuffer[entityName] = {};
	}

	if (gameData->outBuffer[entityName].end() == gameData->outBuffer[entityName].find(getChannel())) {
		gameData->outBuffer[entityName][getChannel()] = {};
	}

	time_t curTime;
	time(&curTime);

	gameData->outBuffer[entityName][getChannel()].push({
		curTime,
		getBufferStr()
	});
}

std::unique_ptr<trogdor::Trogout> PHPStreamOut::clone() {

	std::unique_ptr<PHPStreamOut> stream = std::make_unique<PHPStreamOut>(gameData);

	stream->setEntity(entity);
	return stream;
}
