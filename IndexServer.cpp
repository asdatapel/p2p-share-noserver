//
// Created by Asda Tapel on 1/17/17.
//

#include "IndexServer.h"

IndexServer::~IndexServer(){
	for (int i = 0; i < clients.size(); ++i){
		delete clients[i];
	}
}

void IndexServer::init() {
	listener.listen(serverPort); //random port number
	waiter.add(listener);
}

void IndexServer::go() {
	while (waiter.wait()) {

		//check if new connection
		if (waiter.isReady(listener)) {
			Connection *newPeer = new Connection();
			listener.accept(newPeer->socket);
			newPeer->ip = newPeer->socket.getRemoteAddress();
			newPeer->port = newPeer->socket.getRemotePort();
			clients.push_back(newPeer);
			waiter.add(newPeer->socket);
		}

		//check if something from existing connection
		for (int i = 0; i < clients.size(); ++i) {
			if (waiter.isReady(clients[i]->socket)) {
				handleMessage(clients[i]);
			}
		}
	}
}

void IndexServer::handleMessage(Connection *source) {
	sf::Packet packet;
	source->socket.receive(packet);

	sf::Int32 message_type;
	packet >> message_type;

	if (message_type == SERVER_REQUEST_FILE_LOCATION) {
		std::string filename;
		packet >> filename;

		Connection *location = getFileLocation(filename);
		if (location) {
			sf::Packet response;
			response << CLIENT_GIVE_FILE_LOCATION;
			response << location->ip.toString() << location->port;

			source->socket.send(response);
		}
	} else if (message_type == SERVER_REGISTER_FILE) {
		std::string filename;
		packet >> filename;

		files.push_back({filename, source});
	} else if (message_type == SERVER_UNREGISTER_FILE) {
		std::string filename;
		packet >> filename;

		removeFile(filename, source);
	}
}

Connection* IndexServer::getFileLocation(std::string filename) {
	for (int i = 0; i < files.size(); ++i){
		if (filename == files[i].filename){
			return files[i].connection;
		}
	}
	return nullptr;
}

void IndexServer::removeFile(std::string filename, Connection* client) {
	for (int i = 0; i < files.size(); ++i){
		if (filename == files[i].filename && client == files[i].connection){
			files.erase(files.begin() + i);
			return;
		}
	}
}