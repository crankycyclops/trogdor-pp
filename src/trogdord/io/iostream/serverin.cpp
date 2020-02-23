#include "../../include/exception/serverexception.h"
#include "../../include/io/iostream/serverin.h"


trogdor::Trogin &ServerIn::operator>> (std::string &val) {

	if (!entityPtr) {
		throw ServerException("Entity pointer not set on input stream before use. This is a bug.");
	}

	// TODO: just sending null command for now
	val = "";
	return *this;
}

/*****************************************************************************/

std::unique_ptr<trogdor::Trogin> ServerIn::clone() {

	std::unique_ptr<ServerIn> copy = std::make_unique<ServerIn>(gameId);

	copy->setEntity(entityPtr);
	return copy;
}
