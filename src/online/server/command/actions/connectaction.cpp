#include <string>
#include "../../include/command/actions/connectaction.h"


void ConnectAction::execute(TCPConnection::ptr connection) {

	connection->write(std::string(ACKNOWLEDGE) + EOT, freeConnection, 0);
	return;
}
