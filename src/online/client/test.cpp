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

	response.erase(remove(response.begin(), response.end(), EOT), response.end());
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
		std::string test = "NEWPLAYER james";
		test += EOT;
		boost::asio::write(*socket, boost::asio::buffer(test), ignored_error);

		test = "COMMAND james";
		test += EOT;
		boost::asio::write(*socket, boost::asio::buffer(test), ignored_error);

		while (1) {
			std::string response = getResponse(socket);
			std::cout << response << std::endl;
			std::cout.flush();
		}

		delete socket;
	}

	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

