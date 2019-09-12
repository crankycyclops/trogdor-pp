#ifndef TCPOUT_H
#define TCPOUT_H


#include <string>
#include <memory>
#include <ctime>

#include <boost/lexical_cast.hpp>

#include "../network/tcpcommon.h"
#include "../network/tcpconnection.h"
#include "../../../../core/include/iostream/trogout.h"

// maximum number of times to retry sending a message in the event of an error
#define MAX_RETRIES 3


using namespace std;


/*
	Output "stream" that writes data to a TCP connection.

	Each message is sent out in the following format:
		MESSAGE <timestamp> <channel> <message string>

	In the event that we fail to send the message, we'll retry a maximum of
	MAX_RETRIES times. The client is responsible for using the timestamp and
	responsible buffering to ensure that messages are always received in
	order. One possible approach that the client could take:

	1. client receives message and places it on a priority queue ordered by the
	   timestamp.
	2. Every n milliseconds, pop each message from the queue and display it.

	Note that the client would have to ensure that buffered messages are
	flushed before each clock tick.

	TODO: to handle errors, we'll have to modify the arguments that get passed
	to handleWrite in TCPConnection so that if there's an error, we can call
	a different custom callback. This callback would take as an argument both
	the connection object and the message struct, and would allow us to retry
	sending the message up to MAX_RETRIES times.
*/
class TCPOut: public core::Trogout {

	private:

		TCPConnection::ptr connection;

		// structure of a message that gets written to a TCP connection
		struct Message {
			time_t timestamp;
			string channel;
			string message;
		};

		// Crafts a message command for the client from a Message struct.
		inline std::string makeMessageCommand(Message m) {

			return std::string("MESSAGE ") +
				boost::lexical_cast<std::string>(m.timestamp) + " " +
				m.channel + " " + m.message + EOT;
		}

		// Handles error checking 
		static void handleMessageWrite(TCPConnection::ptr connection, void *message);

	public:

		// Constructor (Set a null connection, because we have to set it
		// manually every time we use this stream object)
		inline TCPOut(TCPConnection::ptr c = TCPConnection::ptr()): connection(c) {}

		// Allows us to use a new TCP connection object in the event that a
		// previous connection expires.
		inline void setConnection(TCPConnection::ptr c) {connection = c;}

		/*
			See include/iostream/trogout.h for details.
		*/
		virtual void flush();

		/*
			See include/iostream/trogin.h for details.
		*/
		virtual std::unique_ptr<core::Trogout> clone();
};


#endif

