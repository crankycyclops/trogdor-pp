#ifndef BADCOMMANDACTION_H
#define BADCOMMANDACTION_H


#include "../networkaction.h"


// Thrown when the client provides an unknown command.
class BadCommandAction: public NetworkAction {

	private:

		// Sends the acknowledgement
		virtual void execute(TCPConnection::ptr connection);
};


#endif

