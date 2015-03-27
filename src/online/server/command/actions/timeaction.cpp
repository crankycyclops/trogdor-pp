#include <string>
#include <boost/lexical_cast.hpp>

#include "../../include/network/tcpserver.h"
#include "../../include/command/actions/timeaction.h"

// provides extern definition of Game object
#include "../../include/main.h"


void TIMEAction::execute(TCPConnection::ptr connection) {

	connection->write(std::string() +
		boost::lexical_cast<std::string>(currentGame->getTime()) + EOT, freeConnection, 0);
	return;
}

