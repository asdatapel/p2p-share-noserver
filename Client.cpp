#include "Client.h"

//just need to clean out the sockets
Client::~Client() {
	for (int i = 0; i < peers.size(); ++i) {
		peers[i]->socket.disconnect();
		delete peers[i];
	}
}

void Client::init() {
	timeToExit = false;

	std::cout << "Starting Client" << "\n";
	indexServer.socket.connect(sf::IpAddress::LocalHost, serverPort); //need error check

	//TODO: register all files with server

	listener.listen(60000); //random port number
	waiter.add(listener);
	waiter.add(indexServer.socket);
}

void Client::go() {
	std::thread loopThread(&Client::incomingLoop, this);

	inputLoop();

	loopThread.join();

}

//this is the loop that takes in user commands
void Client::inputLoop() {
	std::cout << "Ready\n";
	while (!timeToExit) {
		std::string input;
		std::getline(std::cin, input);

		handleInput(input);
	}
}

void Client::handleInput(std::string input) {
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
	} else if (commandParts[0] == "getfileloc") {
		sf::Packet message;
		message << SERVER_REQUEST_FILE_LOCATION;
		message << commandParts[1];

		std::cout << "Requesting file \"" << commandParts[1] << "\"\n";

		indexServer.socket.send(message);
	} else if (commandParts[0] == "addfile") {
		sf::Packet message;
		message << SERVER_REGISTER_FILE;
		message << commandParts[1];

		std::cout << "Registering file \"" << commandParts[1] << "\"\n";

		indexServer.socket.send(message);
	}

	lock.unlock();
}

//this is the loop that waits for messages and handles them
void Client::incomingLoop() {
	while (true) {    //wait for message to come in or new connection
		bool anythingReady = waiter.wait(sf::seconds(2));

		lock.lock();
		if (!anythingReady) {
			if (timeToExit) {
				lock.unlock();
				return;
			}
		} else {
			//check if new connection
			if (waiter.isReady(listener)) {
				Connection *newPeer = new Connection();
				listener.accept(newPeer->socket);

				newPeer->ip = newPeer->socket.getRemoteAddress();
				newPeer->port = newPeer->socket.getRemotePort();

				peers.push_back(newPeer);
				waiter.add(newPeer->socket);
			}

			//check if something from a connected peer
			for (int i = 0; i < peers.size(); ++i) {
				if (waiter.isReady(peers[i]->socket)) {
					handleMessage(peers[i]);
				}
			}

			//check if something from the server
			if (waiter.isReady(indexServer.socket)) {
				handleMessage(&indexServer);
			}


		}
		lock.unlock();
	}
}

void Client::handleMessage(Connection *source) {
	//TODO: handle incoming message, similar to IndexServer::handleMessage

	sf::Packet packet;
	sf::Socket::Status status = source->socket.receive(packet);
	if (status != sf::Socket::Done) {
		//TODO: do some error stuff
		return;
	}

	sf::Int32 message_type;
	packet >> message_type;

	if (message_type == CLIENT_GIVE_FILE_LOCATION) {
		std::string ip;
		int port;
		packet >> ip >> port;

		std::cout << "File location: " << ip << ", " << port << "\n";
	}
}
