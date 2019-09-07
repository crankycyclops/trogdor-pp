#ifndef TCPIN_H
#define TCPIN_H


#include <cstddef>
#include <thread>
#include <mutex>

#include "../network/tcpconnection.h"

#include "../../../../core/include/iostream/trogin.h"


using namespace std;


/*
 Input "stream" that uses data from a TCP connection's read buffer.

 WARNING: ALWAYS use TCPIn inside a separate thread, because it WILL block while
 it waits on the client for data.
*/
class TCPIn: public core::Trogin {

	private:

		TCPConnection::ptr connection;

		// Not really a mutex, but a way to signal when an asynchronous request
		// for input is complete.
		bool requestMutex;

		// Blocks any thread that tries to access a particular instance of
		// TCPIn while another thread is currently using it.
		std::mutex streamMutex;

		// Callback that asynchronously reads data from the client after the
		// it recives the GET command. Takes as an argument a pointer to the
		// current instance of TCPIn.
		static void readInput(TCPConnection::ptr connection, void *that);

		// Callback that signals to the input stream operator that data from
		// the client has arrived and that it can continue executing. Takes as
		// input a pointer to the current instance of TCPIn.
		static void processInput(TCPConnection::ptr connection, void *that);

	public:

		// Constructor (Set a null connection, because we have to set it
		// manually every time we use this stream object)
		inline TCPIn(TCPConnection::ptr c = TCPConnection::ptr()):
			connection(c), requestMutex(false) {}

		// Destructor
		inline ~TCPIn() {}

		// Allows us to use a new TCP connection object in the event that a
		// previous connection expires.
		inline void setConnection(TCPConnection::ptr c) {connection = c;}

		/*
		 See include/iostream/trogin.h for details.
		*/
		virtual core::Trogin *clone();

		// For now, I only need to define input for strings.
		virtual core::Trogin &operator>> (string &val);
};


#endif

