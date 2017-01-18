//
// Created by Asda Tapel on 1/17/17.
//

#ifndef P2P_SHARE_SERVER_H
#define P2P_SHARE_SERVER_H

#include <vector>

#include <SFML/Network.hpp>

#include "Const.h"
#include "Connection.h"

struct IndexFile{
	std::string filename;
	Connection* connection;
	//int size; not necessary
};

class IndexServer {
public:
	~IndexServer();

	void init();

	void go();
private:
	std::vector<Connection*> clients;
	sf::TcpListener listener;
	sf::SocketSelector waiter;

	Connection* getFileLocation(std::string filename);

	void removeFile(std::string filename, Connection* peer);

	void handleMessage(Connection* source);

	std::vector<IndexFile> files;
};


#endif //P2P_SHARE_SERVER_H
