#include "IndexServer.h"

IndexServer::IndexServer() {
	timeToExit = false;
}

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
}

//this is the main loop that waits for messages and handles them
void IndexServer::go() {
	std::thread loopThread(&IndexServer::incomingLoop, this);

	inputLoop();

	loopThread.join();
}


//this is the loop that takes in user commands
void IndexServer::inputLoop() {
	std::cout << "Server Ready\n";
	while (!timeToExit) {
		std::string input;
		std::getline(std::cin, input);

		handleInput(input);
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

void IndexServer::handleInput(std::string input) {
	std::vector<std::string> commandParts;

	//parse the input, should probably be in own function
	std::replace(input.begin(), input.end(), '\n', ' ');
	std::stringstream stream(input);
	std::string part;
	int i = 0;
	while (std::getline(stream, part, ' ')) {
		commandParts.push_back(part);
		++i;
	}

	lock.lock();

	if (commandParts[0] == "exit") {
		timeToExit = true;
	}

	lock.unlock();
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


void IndexServer::incomingLoop() {
	while (true) {
		bool anythingReady = waiter.wait(sf::seconds(2)); //wait for message to come in or new connection

		lock.lock();
		if (!anythingReady) { // check if something actually came in
			if (timeToExit) {
				lock.unlock();
				return;
			}
		} else {
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
		lock.unlock();
	}
}