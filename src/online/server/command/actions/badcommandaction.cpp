#include <string>
#include "../../include/network/tcpserver.h"
#include "../../include/command/actions/badcommandaction.h"


void BadCommandAction::execute(TCPConnection::ptr connection) {

	connection->write(std::string(BADCOMMAND) + EOT, 0, 0);
	connection->close();
	connection->getServer()->removeActiveConnection(connection);

	return;
}

