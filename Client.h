#ifndef P2P_SHARE_CLIENT_H
#define P2P_SHARE_CLIENT_H

#include <SFML/Network.hpp>

#include "Const.h"
#include "Connection.h"

class Client {
public:
	~Client();

	void init();
private:
	std::vector<Connection*> peers;
	sf::TcpListener listener;
	sf::SocketSelector waiter;

	Connection indexServer;
};


#endif //P2P_SHARE_CLIENT_H
