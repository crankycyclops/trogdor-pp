#include <stdexcept>
#include <future>
#include <chrono>

#include "tcpconnection.h"


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

	try {

		tcp::resolver resolver(io);
		tcp::resolver::query query(hostname, std::to_string(port));
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		socket = std::make_unique<boost::asio::ip::tcp::socket>(io);

		// Attempt to connect, but timeout if it takes too long
		std::future_status status = std::async(std::launch::async, [&] () {

			boost::asio::connect(*socket, endpoint_iterator);
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
		throw std::runtime_error(e.what());
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
		throw std::runtime_error(e.what());
	}

	if (error) {
		throw std::runtime_error(error.message());
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
		throw std::runtime_error(e.what());
	}

	if (error) {
		throw std::runtime_error(error.message());
	}
}
