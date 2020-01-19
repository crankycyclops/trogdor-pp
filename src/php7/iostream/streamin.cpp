#include "streamin.h"


trogdor::Trogin &PHPStreamIn::operator>> (std::string &val) {

	// TODO: read from input buffer
	return *this;
}

std::unique_ptr<trogdor::Trogin> PHPStreamIn::clone() {

	std::unique_ptr<PHPStreamIn> stream = std::make_unique<PHPStreamIn>(gameData);

	stream->setEntity(entity);
	return stream;
}
