#ifndef P2P_SHARE_PEER_H
#define P2P_SHARE_PEER_H

#include <list>
#include <string>

#include <SFML/Network.hpp>

/* This class is redundant right now,
 * since sf::TcpSocket contains the
 * ip and port but it might be
 * useful later to include things like bandwidth, ping, etc.*/

class Connection {
public:
	sf::TcpSocket socket;
	sf::IpAddress ip;
	sf::Uint32 port;
};


#endif //P2P_SHARE_PEER_H
