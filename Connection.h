#ifndef P2P_SHARE_PEER_H
#define P2P_SHARE_PEER_H

#include <list>
#include <string>

#include <SFML/Network.hpp>

class Connection {
public:

	Connection(){
		isConnected = false;
	}

	sf::TcpSocket socket;
	sf::IpAddress ip;
	sf::Uint32 port;

	bool isConnected;
};


#endif //P2P_SHARE_PEER_H
