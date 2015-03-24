#ifndef NETWORKACTION_H
#define NETWORKACTION_H


#include "../network/tcpcommon.h"
#include "../network/tcpconnection.h"


class NetworkAction {

	private:

		// Each child of NetworkAction will deal with a different kind of request.
		virtual void execute(TCPConnection::ptr connection) = 0;

	public:

		inline void operator()(TCPConnection::ptr connection) {execute(connection);}
};


#endif

