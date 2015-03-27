#ifndef ISGAMEONACTION_H
#define ISGAMEONACTION_H


#include "../networkaction.h"


// Returns whether or not a game is currently running.
class ISGAMEONAction: public NetworkAction {

	private:

		virtual void execute(TCPConnection::ptr connection);
};


#endif
