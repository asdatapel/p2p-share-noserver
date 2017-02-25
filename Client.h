#ifndef P2P_SHARE_CLIENT_H
#define P2P_SHARE_CLIENT_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>

#include <SFML/Network.hpp>

#include "Const.h"
#include "Connection.h"
#include "File.h"

class Client {
public:
	Client();
	~Client();

	void init();

	void go();
private:
	std::vector<Connection*> peers;
	sf::TcpListener listener;
	sf::SocketSelector waiter;

	std::mutex lock;

	std::vector<File> incompleteFiles;
	File* findIncompleteFile(std::string filename);
	void removeIncompleteFile(std::string filename);

	std::string myIp;
	sf::Uint32 listenerPort;

	bool timeToExit;

	void incomingLoop();
	void inputLoop();
	void readConfigFile();

	void handleMessage(Connection* peer);
	void handleInput(std::string input);

	void handleQuit();

	Connection* connectToPeer(std::string ip, sf::Uint32 port);
	Connection* findPeer(std::string ip, sf::Uint32 port);

	bool currentlyTestingServer;
	int pendingResponses;
	sf::Clock timer;

};


#endif //P2P_SHARE_CLIENT_H
