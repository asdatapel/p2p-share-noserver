#include "IndexServer.h"

//just need to clean out the sockets
IndexServer::~IndexServer() {
	for (int i = 0; i < clients.size(); ++i) {
		clients[i]->socket.disconnect();
		delete clients[i];
	}
}

void IndexServer::init() {
	std::cout << "Starting Server" << "\n";
	listener.listen(serverPort); //start the listener
	waiter.add(listener);

	std::cout << "Server ready\n";
}

//this is the main loop that waits for messages and handles them
//right now its an infinite loop, need to add termination
void IndexServer::go() {
	while (waiter.wait()) {    //wait for message to come in or new connection

		//check if new connection
		if (waiter.isReady(listener)) {
			Connection *newClient = new Connection();
			listener.accept(newClient->socket);

			newClient->ip = newClient->socket.getRemoteAddress();
			newClient->port = newClient->socket.getRemotePort();

			std::cout << "New connection: IP " << newClient->ip << ", Port " << newClient->port << "\n";

			clients.push_back(newClient);
			waiter.add(newClient->socket);
		}

		//check if something from existing connection
		for (int i = 0; i < clients.size(); ++i) {
			if (waiter.isReady(clients[i]->socket)) {
				handleMessage(clients[i]);
			}
		}
	}
}

//handle all requests from a client
void IndexServer::handleMessage(Connection *source) {
	sf::Packet packet;
	sf::Socket::Status status = source->socket.receive(packet);
	if (status != sf::Socket::Done) {
		//TODO: do some error stuff
		return;
	}

	sf::Int32 message_type;
	packet >> message_type;

	if (message_type == SERVER_REQUEST_FILE_LOCATION) {
		std::string filename;
		packet >> filename;

		std::cout << "Request for file \"" << filename << "\"\n";

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

		std::cout << "New file \"" << filename << "\"\n";

		files.push_back({filename, source});
	} else if (message_type == SERVER_UNREGISTER_FILE) {
		std::string filename;
		packet >> filename;

		removeFile(filename, source);
	}
	//TODO: handle all types of messages
}

//returns the Connection that owns the requested file
//if not found, returns nullptr
Connection *IndexServer::getFileLocation(std::string filename) {
	for (int i = 0; i < files.size(); ++i) {
		if (filename == files[i].filename) {
			return files[i].connection;
		}
	}
	return nullptr;
}

void IndexServer::removeFile(std::string filename, Connection *client) {
	for (int i = 0; i < files.size(); ++i) {
		if (filename == files[i].filename && client == files[i].connection) {
			files.erase(files.begin() + i);
			return;
		}
	}
}