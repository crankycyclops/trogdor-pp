#include "tcpconnectionmap.h"


// Singleton instance
std::unique_ptr<TCPConnectionMap> TCPConnectionMap::instance = nullptr;

/*****************************************************************************/

TCPConnectionMap &TCPConnectionMap::get() {

	if (!instance) {
		instance = std::unique_ptr<TCPConnectionMap>(new TCPConnectionMap());
	}

	return *instance;
}

/*****************************************************************************/

TCPConnection &TCPConnectionMap::connect(std::string hostname, unsigned short port) {

	const std::string key = hostname + ":" + std::to_string(port);

	if (connections.end() == connections.find(key)) {
		connections[key] = std::make_unique<TCPConnection>(hostname, port);
	}

	return *connections[key];
}
