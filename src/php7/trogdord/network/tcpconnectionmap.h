#ifndef PHP_TCPCONNECTIONMAP_H
#define PHP_TCPCONNECTIONMAP_H

#include <unordered_map>
#include "tcpconnection.h"


class TCPConnectionMap {

	private:

		// Singleton instance
		static std::unique_ptr<TCPConnectionMap> instance;

		// Maps "<hostname>:<port>" to an instance of TCPConnection
		std::unordered_map<std::string, std::unique_ptr<TCPConnection>> connections;

		// Private constructor ensures we can only use a singleton instance
		inline TCPConnectionMap() {}
		TCPConnectionMap(const TCPConnectionMap &) = delete;

	public:

		// Returns singleton instance
		static TCPConnectionMap &get();

		// Returns a connection to the specified hostname or IP address and
		// port
		TCPConnection &connect(std::string hostname, unsigned short port);
};


#endif
