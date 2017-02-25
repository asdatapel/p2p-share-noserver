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

	Connection(std::string ip, int port){
		this->ip = ip;
		this->port = port;
	}

	sf::TcpSocket socket;
	std::string ip;
	sf::Uint32 port;

	bool isConnected;

	std::string toString(){
		return "IP: " + ip + ", Port: " + std::to_string(port);
	}
};


#endif //P2P_SHARE_PEER_H
