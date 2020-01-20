#include "streamin.h"


trogdor::Trogin &PHPStreamIn::operator>> (std::string &val) {

	if (!entity) {
		return *this;
	}

	const std::string &entityName = entity->getName();
	std::unique_ptr<customConstructedData> &cd = customConstructedDataMap[gameData->obj];

	// Wait until input is available in the buffer
	std::unique_lock<std::mutex> lock(cd->inputConditionMutex);
	cd->inputCondition.wait(lock, [&]() {

std::cout << "input condition met!" << std::endl;

		if (cd->inBuffer.end() == cd->inBuffer.find(entity->getName())) {
			return false;
		} else {
			return !cd->inBuffer[entity->getName()].empty();
		}
	});

	cd->inBufferMutex.lock();
	val = cd->inBuffer[entity->getName()].front();
	cd->inBuffer[entity->getName()].pop();
	cd->inBufferMutex.unlock();

	return *this;
}

std::unique_ptr<trogdor::Trogin> PHPStreamIn::clone() {

	std::unique_ptr<PHPStreamIn> stream = std::make_unique<PHPStreamIn>(gameData);

	stream->setEntity(entity);
	return stream;
}
