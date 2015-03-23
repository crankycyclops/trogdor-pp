#include "../network/tcpcommon.h"
#include "../network/tcpconnection.h"
#include "../network/tcpserver.h"


// root callback that answers new client-issued commands
extern void serveConnection(TCPConnection::ptr connection, void *);

