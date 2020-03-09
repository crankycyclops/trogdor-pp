#include <stdexcept>
#include <future>
#include <chrono>

#include "tcpconnection.h"


// What trogdord writes to the socket as soon as the connection is opened. Used
// to check whether or not a new connection was successful.
static const char *READY_RESPONSE = "{\"status\":\"ready\"}";

/*****************************************************************************/

TCPConnection::TCPConnection(std::string h, unsigned short p): hostname(h), port(p) {

	open();
}

/*****************************************************************************/

TCPConnection::TCPConnection(const TCPConnection &original) {

	hostname = original.hostname;
	port = original.port;

	open();
}

/*****************************************************************************/

TCPConnection &TCPConnection::operator=(const TCPConnection &original) {

	if (this != &original) {
		*this = TCPConnection(original);
	}

	return *this;
}

/*****************************************************************************/

TCPConnection::~TCPConnection() {

	close();
}

/*****************************************************************************/

void TCPConnection::open() {

	boost::system::error_code error = boost::asio::error::host_not_found;

	try {

		tcp::resolver resolver(io);
		tcp::resolver::query query(hostname, std::to_string(port));

		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;

		socket = std::make_unique<boost::asio::ip::tcp::socket>(io);

		// Attempt to connect, but timeout if it takes too long
		std::future_status status = std::async(std::launch::async, [&] () {

			// We might get IPv4 and IPv6 endpoints. This way, we can try
			// each and see which one works.
			while (error && endpoint_iterator != end) {

				socket->close();
				socket->connect(*endpoint_iterator++, error);

				// If the endpoint is found, make sure there's actually
				// an instance of trogdord listening on it.
				try {

					std::string response = read();

					if (0 != response.compare(READY_RESPONSE)) {
						error = boost::asio::error::not_connected;
					}
				}

				catch (const std::runtime_error &e) {
					error = boost::asio::error::not_connected;
				}
			}

		}).wait_for(std::chrono::milliseconds{TIMEOUT});

		switch (status) {

			case std::future_status::deferred:
				std::logic_error("Bug occurred in TCPConnection::open().");
				break;

			case std::future_status::timeout:
				timeout();
				break;

			default:
				break;
		}
	}

	catch (const boost::system::system_error &e) {
		close();
		throw NetworkException(e.what());
	}

	if (error) {
		close();
		throw NetworkException(error.message());
	}
}

/*****************************************************************************/

std::string TCPConnection::read() {

	boost::asio::streambuf buffer;
	boost::system::error_code error;

	try {

		std::future_status status = std::async(std::launch::async, [&] () {

			boost::asio::read_until(*socket, buffer, std::string() + EOT, error);
		}).wait_for(std::chrono::milliseconds{TIMEOUT});

		switch (status) {

			case std::future_status::deferred:
				std::logic_error("Bug occurred in TCPConnection::read().");
				break;

			case std::future_status::timeout:
				timeout();
				break;

			default:
				break;
		}
	}

	catch (const boost::system::system_error &e) {
		close();
		throw NetworkException(e.what());
	}

	if (error) {
		close();
		throw NetworkException(error.message());
	}

	std::string response = boost::asio::buffer_cast<const char *>(buffer.data());
	response.erase(remove(response.begin(), response.end(), EOT), response.end());

	return response;
}

/*****************************************************************************/

void TCPConnection::write(std::string message) {

	boost::system::error_code error;

	try {

		std::future_status status = std::async(std::launch::async, [&] () {

			boost::asio::write(*socket, boost::asio::buffer(message + EOT), error);
		}).wait_for(std::chrono::milliseconds{TIMEOUT});

		switch (status) {

			case std::future_status::deferred:
				std::logic_error("Bug occurred in TCPConnection::write().");
				break;

			case std::future_status::timeout:
				timeout();
				break;

			default:
				break;
		}
	}

	catch (const boost::system::system_error &e) {
		close();
		throw NetworkException(e.what());
	}

	if (error) {
		close();
		throw NetworkException(error.message());
	}
}
