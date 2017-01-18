//
// Created by Asda Tapel on 1/17/17.
//

#ifndef P2P_SHARE_PEER_H
#define P2P_SHARE_PEER_H

#include <list>
#include <string>

#include <SFML/Network.hpp>

class Connection {
public:
	sf::TcpSocket socket;
	sf::IpAddress ip;
	sf::Uint32 port;
};


#endif //P2P_SHARE_PEER_H
