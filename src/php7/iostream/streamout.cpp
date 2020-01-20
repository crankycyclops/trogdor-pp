#include "streamout.h"


void PHPStreamOut::flush() {

	if (!entity) {
		return;
	}

	const std::string &entityName = entity->getName();
	std::unique_ptr<customConstructedData> &cd = customConstructedDataMap[gameData->obj];

	time_t curTime;
	time(&curTime);

	cd->outBufferMutex.lock();

	if (cd->outBuffer.end() == cd->outBuffer.find(entityName)) {
		cd->outBuffer[entityName] = {};
	}

	if (cd->outBuffer[entityName].end() == cd->outBuffer[entityName].find(getChannel())) {
		cd->outBuffer[entityName][getChannel()] = {};
	}

	cd->outBuffer[entityName][getChannel()].push({
		curTime,
		getBufferStr()
	});

	cd->outBufferMutex.unlock();
	clear();
}

std::unique_ptr<trogdor::Trogout> PHPStreamOut::clone() {

	std::unique_ptr<PHPStreamOut> stream = std::make_unique<PHPStreamOut>(gameData);

	stream->setEntity(entity);
	return stream;
}
