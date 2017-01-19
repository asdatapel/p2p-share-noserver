#ifndef P2P_SHARE_CLIENT_H
#define P2P_SHARE_CLIENT_H

#include <vector>
#include <thread>
#include <mutex>

#include <SFML/Network.hpp>

#include "Const.h"
#include "Connection.h"

class Client {
public:
	~Client();

	void init();

	void go();
private:
	std::vector<Connection*> peers;
	Connection indexServer;
	sf::TcpListener listener;
	sf::SocketSelector waiter;

	std::mutex lock;

	void incomingLoop();

	void handleMessage(Connection* peer);

};


#endif //P2P_SHARE_CLIENT_H
