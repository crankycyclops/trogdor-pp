#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../include/network/tcpcommon.h"
#include "../include/iostream/tcpin.h"


using namespace std;


void TCPIn::processInput(TCPConnection::ptr connection, void *that) {

	((TCPIn *)that)->requestMutex = true;
}


void TCPIn::readInput(TCPConnection::ptr connection, void *that) {

	connection->read(TCPIn::processInput, that);
}


// WARNING: always do this inside a separate thread, because TCPIn WILL block.
core::Trogin &TCPIn::operator>> (string &val) {

	// if there's no connection, then we can't really get anything meaningful
	if (!connection.get()) {
		val = "";
	}

	else {

		streamMutex.lock();
		connection->write(string("GET") + EOT, TCPIn::readInput, (void *)this);

		while (!requestMutex) {
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}

		val = connection->getBufferStr();

		requestMutex = false; // reset this for the next call
		streamMutex.unlock();

		return *this;
	}
}


core::Trogin *TCPIn::clone() {

	return new TCPIn(connection);
}

