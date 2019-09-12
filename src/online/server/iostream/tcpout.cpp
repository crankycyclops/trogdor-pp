#include <ctime>
#include "../include/iostream/tcpout.h"


using namespace std;


// TODO: need to alter connection so it can have an error callback
void TCPOut::handleMessageWrite(TCPConnection::ptr connection, void *message) {

	// TODO: handle on error retries (requires error handler in TCPConnection)

	// NOTE: do not call connection->setInUse(false) here. The inUse should only
	// be used when processing incoming requests. The client should understand
	// that it might arbitrarily receive MESSAGE commands, and it should know how
	// to handle that.
}


void TCPOut::flush() {

	// we can't actually write anything unless we have a connection
	if (connection.get()) {

		connection->setInUse(true);

		Message *m = new Message();
		time(&m->timestamp);
		m->channel = getChannel();
		m->message = getBufferStr();

		connection->write(makeMessageCommand(*m), TCPOut::handleMessageWrite, m);
	}

	clear();
}


std::unique_ptr<core::Trogout> TCPOut::clone() {

	return std::make_unique<TCPOut>(connection);
}

