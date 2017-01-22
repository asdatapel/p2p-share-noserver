#ifndef P2P_SHARE_SERVER_H
#define P2P_SHARE_SERVER_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>

#include <SFML/Network.hpp>

#include "Const.h"
#include "Connection.h"

struct IndexFile {
	std::string filename;
	Connection *connection;

	/*this stuff maybe not necessary*/
	//int size;
	//std::string filetype;

};

class IndexServer {
public:
	IndexServer();
	~IndexServer();

	void init();

	void go();
private:
	std::vector<Connection *> clients;
	sf::TcpListener listener;
	sf::SocketSelector waiter;

	bool timeToExit;

	std::mutex lock;

	Connection *getFileLocation(std::string filename);

	void removeFile(std::string filename, Connection *peer);

	void incomingLoop();
	void inputLoop();

	void handleMessage(Connection *source);
	void handleInput(std::string input);

	std::vector<IndexFile> files;

	void handleQuit();

};

#endif //P2P_SHARE_SERVER_H
