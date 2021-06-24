#ifndef CONNECTION_FACTORY_H
#define CONNECTION_FACTORY_H

#include <thread>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>

#include <trogdord/network/tcpcommon.h>
#include <trogdord/network/tcpconnection.h>
#include <trogdord/network/tcpserver.h>


// This class manufactures test instances of TCPConnection for the purposes of
// unit testing.
class ConnectionFactory {

	private:

		bool started = false;

		// IO service and accompanying instance of TCPServer
		boost::asio::io_service io;
		std::unique_ptr<TCPServer> server;

		// Thread that will be used to run io.run()
		std::thread IOServiceThread;
		std::mutex IOServiceMutex;

	protected:

		static std::unique_ptr<ConnectionFactory> instance;

		// Constructor
		ConnectionFactory();
		ConnectionFactory(const ConnectionFactory &) = delete;

	public:

		// Destructor
		~ConnectionFactory();

		// Returns singleton instance of ConnectionFactory.
		static std::unique_ptr<ConnectionFactory> &get();

		// Returns true if the ConnectionFactory has been started and fals if
		// not.
		inline bool isStarted() {return started;}

		// Starts the ConnectionServer and its internal instance of TCPServer,
		// which will listen on the specified port.
		void start(size_t port = 1041);

		// Stops the ConnectionServer. In this state, test connections cannot
		// be manufactured and any previously made connections will be invalid.
		void stop();
};


#endif