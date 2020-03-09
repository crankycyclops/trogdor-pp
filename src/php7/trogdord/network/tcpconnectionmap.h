#ifndef PHP_TCPCONNECTIONMAP_H
#define PHP_TCPCONNECTIONMAP_H

#include <unordered_map>
#include "tcpconnection.h"


class TCPConnectionMap {

	private:

		typedef std::unordered_map<std::string, std::unique_ptr<TCPConnection>> HostToConnection;

		// Singleton instance
		static std::unique_ptr<TCPConnectionMap> instance;

		// Maps "<hostname>:<port>" to an instance of TCPConnection
		HostToConnection connections;

		// Private constructor ensures we can only use a singleton instance
		inline TCPConnectionMap() {}
		TCPConnectionMap(const TCPConnectionMap &) = delete;

	public:

		// Returns singleton instance
		static TCPConnectionMap &get();

		// Returns a connection to the specified hostname or IP address and
		// port
		TCPConnection &connect(std::string hostname, unsigned short port);

		// Clears all connections, closing them if necessary
		inline void clear() {connections.clear();}

		// Allows iteration over all connections with a for-range loop
		inline HostToConnection::const_iterator const begin() const {return connections.cbegin();}
		inline HostToConnection::const_iterator const end() const {return connections.cend();}
};


#endif
