#ifndef CONNECTACTION_H
#define CONNECTACTION_H


#include "../networkaction.h"


// Sends confirmation to the client to acknowledge a new connection.
class ConnectAction: public NetworkAction {

	private:

		// Free up the connection so that it can be used for other requests.
		static void acknowledgeCallback(TCPConnection::ptr connection, void *);

		// Sends the acknowledgement
		virtual void execute(TCPConnection::ptr connection);
};


#endif

