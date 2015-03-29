#include <ctime>
#include "../include/iostream/tcpout.h"


using namespace std;


// TODO: need to alter connection so it can have an error callback
void TCPOut::handleMessageWrite(TCPConnection::ptr connection, void *message) {

	// TODO: handle on error retries (requires error handler in TCPConnection)
	connection->setInUse(false);
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


core::Trogout *TCPOut::clone() {

	return new TCPOut(connection);
}

