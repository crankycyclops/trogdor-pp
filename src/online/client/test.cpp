// Compile with: g++ tcp1.cpp -o client -lboost_system -pthread

#include <iostream>
#include <string>
#include <cstdlib>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

char EOT = 0x04;


tcp::socket *getSocket(const char *ipOrDomain, const char *service) {

	boost::asio::io_service io_service;

	tcp::resolver resolver(io_service);
	tcp::resolver::query query(ipOrDomain, service);

	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

	tcp::socket *socket = new tcp::socket(io_service);
	boost::asio::connect(*socket, endpoint_iterator);

	return socket;
}


std::string getResponse(tcp::socket *socket) {

	boost::asio::streambuf acceptBuffer;
	boost::system::error_code error;

	boost::asio::read_until(*socket, acceptBuffer, std::string() + EOT, error);
	std::string response = boost::asio::buffer_cast<const char *>(acceptBuffer.data());

	// remove EOT
	response.resize (response.size() - 1);

	return response;
}


int main(int argc, char **argv) {

	try {

		if (argc != 2) {
			std::cerr << "Trogdor test client usage: cluent <host>" << std::endl;
			return EXIT_FAILURE;
		}

		tcp::socket *socket = getSocket(argv[1], "1040");

		// receive acknowledgement that the connection was established
		std::string response = getResponse(socket);
		std::cout << "Making connection...\nServer says: " << response << std::endl;
		std::cout.flush();

		boost::system::error_code ignored_error;
		std::string test = "TEST";
		test += EOT;
		boost::asio::write(*socket, boost::asio::buffer(test), ignored_error);

		while (1) {

			boost::array<char, 128> buf;
			boost::system::error_code error;


			size_t len = socket->read_some(boost::asio::buffer(buf), error);

			if (error == boost::asio::error::eof) {
				break;
			} else if (error) {
				throw boost::system::system_error(error);
			}

			std::cout.write(buf.data(), len);
		}

		delete socket;
	}

	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

