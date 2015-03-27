#ifndef CONNECTACTION_H
#define CONNECTACTION_H


#include "../networkaction.h"


// Sends confirmation to the client to acknowledge a new connection.
class ConnectAction: public NetworkAction {

	private:

		// Sends the acknowledgement
		virtual void execute(TCPConnection::ptr connection);
};


#endif

