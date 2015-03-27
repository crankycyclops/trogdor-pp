#ifndef NETWORKACTION_H
#define NETWORKACTION_H


#include "../network/tcpcommon.h"
#include "../network/tcpconnection.h"


class NetworkAction {

	private:

		// Each child of NetworkAction will deal with a different kind of request.
		virtual void execute(TCPConnection::ptr connection) = 0;

	protected:

		// Marks the connection as no longer in use so that it can be recycled for
		// future requests.
		static void freeConnection(TCPConnection::ptr connection, void *);

	public:

		// Public interface for the private method execute().
		inline void operator()(TCPConnection::ptr connection) {execute(connection);}
};


#endif
