#ifndef NETWORKACTION_H
#define NETWORKACTION_H


#include "../network/tcpcommon.h"
#include "../network/tcpconnection.h"


class NetworkAction {

	private:

		// Increments every time step is called. This enables the action to
		// remember where in the request it is.
		int step;

		// Resets the action object so that it can be used again in another
		// request. Should only be called by operator(). Initialize to -1 so that
		// the subsequent increment operation will make sense.
		inline void reset() {step = -1;}

		// Each child of NetworkAction will deal with a different kind of request.
		virtual void execute(TCPConnection::ptr connection) = 0;

	public:

		// Constructor
		inline NetworkAction(): step(0) {}

		// Call this to abort the request
		inline void abort() {step = 0;}

		inline void operator()(TCPConnection::ptr connection) {execute(connection); step++;}
};


#endif

