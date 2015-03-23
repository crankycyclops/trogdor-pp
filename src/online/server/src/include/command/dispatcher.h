#ifndef DISPATCHER_H
#define DISPATCHER_H


#include "../network/tcpconnection.h"

using namespace std;


class Dispatcher {

	private:

		static Dispatcher *instance;

		// Constructor is private in order to guarantee that we'll be using the
		// singleton model.
		inline Dispatcher() {};

	public:

		// Returns singleton instance of 
		static Dispatcher *get();

		// Locates a root command's corresponding network action and executes it
		void dispatch(TCPConnection::ptr connection);
};


#endif
