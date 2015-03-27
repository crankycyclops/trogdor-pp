#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <boost/unordered_map.hpp>

#include "../network/tcpconnection.h"
#include "../command/networkaction.h"

using namespace std;


class Dispatcher {

	private:

		static Dispatcher *instance;

		// Map of request commands to their associated actions
		boost::unordered_map<string, NetworkAction *> actions;

		// Special NetworkAction we use to confirm to the client that a
		// connection has been made
		NetworkAction *confirmConnect;

		// Special NetworkAction we throw when the client passes an unknown
		// command
		NetworkAction *badCommand;

		// Returns the action associated with the specified command. Returns
		// BadCommandAction if the command is invalid.
		NetworkAction *getAction(const string command) const;

		// Constructor is private in order to guarantee that we'll be using the
		// singleton model
		Dispatcher();

		// Destructor is private and should only be called by the static
		// method destroy()
		~Dispatcher();

	public:

		// Returns singleton instance of 
		static Dispatcher *get();

		// Destroys the Dispatcher object
		static void destroy();

		// Establishes a new connection
		static void establishConnection(TCPConnection::ptr connection, void *);

		// Callback that passes new requests to the dispatcher
		static void serveRequest(TCPConnection::ptr connection, void *);

		// Locates a root command's corresponding network action and executes it
		void dispatch(TCPConnection::ptr connection);
};


#endif
