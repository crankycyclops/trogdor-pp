#ifndef TIMEACTION_H
#define TIMEACTION_H


#include "../networkaction.h"


// Returns the current game time as an integer number of clock ticks.
class TIMEAction: public NetworkAction {

	private:

		virtual void execute(TCPConnection::ptr connection);
};


#endif

