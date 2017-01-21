#ifndef P2P_SHARE_CLIENT_H
#define P2P_SHARE_CLIENT_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <algorithm>

#include <SFML/Network.hpp>

#include "Const.h"
#include "Connection.h"

class Client {
public:
	Client();
	~Client();

	void init();

	void go();
private:
	std::vector<Connection*> peers;
	Connection indexServer;
	sf::TcpListener listener;
	sf::SocketSelector waiter;

	std::mutex lock;

	bool timeToExit;

	void incomingLoop();
	void inputLoop();

	void handleMessage(Connection* peer);
	void handleInput(std::string input);

};


#endif //P2P_SHARE_CLIENT_H
