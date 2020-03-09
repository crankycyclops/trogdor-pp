#ifndef PHP_TCPCONNECTION_H
#define PHP_TCPCONNECTION_H

// Signals the end of a transmission
#define EOT ('\0')

#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class TCPConnection {

	private:

		// Number of milliseconds to wait on a connection before timing out
		const unsigned TIMEOUT = 3000;

		// Hostname or IP address we're connecting to
		std::string hostname;

		// Port that trogdord is listening on
		unsigned short port;

		// The IO service used to read and write data from and to the socket
		boost::asio::io_service io;

		// The underlying socket connection between the client and the server
		std::unique_ptr<boost::asio::ip::tcp::socket> socket;

		// Opens the connection
		void open();

		// Call this whenever a connection times out.
		inline void timeout() {

			close();
			throw std::runtime_error("Timeout after " + std::to_string(TIMEOUT) + "ms.");
		}

	public:

		// Default Constructor
		TCPConnection(std::string hostname, unsigned short port);
		TCPConnection() = delete;

		// Copy constructor and assignment operator
		TCPConnection(const TCPConnection &original);
		TCPConnection &operator=(const TCPConnection &original);

		// Destructor
		~TCPConnection();

		// Read a string from a trogdord instance. Throws an instance of
		// \Trogdord\NetworkException if an error occurs.
		std::string read();

		// Write a string to a trogdord instance. Throws an instance of
		// \Trogdord\NetworkException if an error occurs.
		void write(std::string message);

		// Returns the hostname or ip address we're connecting to
		inline std::string getHostname() {return hostname;}

		// Returns the port trogdord is listening on
		inline unsigned short getPort() {return port;}

		// Whether or not the connection is open.
		inline bool isOpen() {return socket->is_open();}

		// Closes the connection. Once this is done, the connection cannot be
		// reopened again, and any further operations on this connection
		// object beyond calling isOpen() will result in undefined behavior.
		inline void close() {

			if (socket->is_open()) {
				boost::system::error_code ignore;
				socket->shutdown(tcp::socket::shutdown_both, ignore);
				socket->close();
			}
		}
};


#endif
