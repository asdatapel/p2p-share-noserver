#include "Client.h"

Client::Client() {
	timeToExit = false;
	myIp = sf::IpAddress::getPublicAddress(sf::seconds(10)).toString();
	listenerPort = 0; //random port number

	currentlyTestingServer = false;
}

Client::~Client() {
	handleQuit();
}

//start the listener and give it a port
//try to connect to server
void Client::init() {
	std::cout << "Starting Client..." << "\n";

	handleConnectServer(sf::IpAddress::LocalHost.toString(), serverPort);

	listener.listen(listenerPort);
	listenerPort = listener.getLocalPort();
	waiter.add(listener);
	waiter.add(indexServer.socket);
}

//begin both threads
void Client::go() {
	std::thread loopThread(&Client::incomingLoop, this);

	inputLoop();

	loopThread.join();

}

//this is the loop that takes in user commands
void Client::inputLoop() {
	std::cout << "Ready for input.\n";
	while (!timeToExit) {
		std::string input;
		std::getline(std::cin, input);

		handleInput(input);
	}
}

//handle messages from peers
void Client::handleMessage(Connection *peer) {
	sf::Packet packet;
	sf::Socket::Status status = peer->socket.receive(packet);
	if (status != sf::Socket::Done) {
		//TODO: do some error stuff
		return;
	}

	sf::Int32 message_type;
	packet >> message_type;
	if (message_type == PEER_NOTIFY_PEER_DISCONNECT) {
		std::cout << "Peer: {" << peer->toString() << "} disconnected\n";

		waiter.remove(peer->socket);
		peer->socket.disconnect();

		for (int i = 0; i < peers.size(); ++i) {
			if (peers[i] == peer) {
				peers.erase(peers.begin() + i);
				--i;
			}

		}
	} else if (message_type == PEER_REQUEST_FILE) {
		std::string filename;
		packet >> filename;

		File file;
		file.initFromDisk(filename);

		sf::Packet response;
		response << PEER_NOTIFY_STARTING_TRANSFER;
		response << filename << (sf::Uint32) file.size;
		peer->socket.send(response);

		file.send(peer);

		std::cout << "Starting upload file: \"" << filename << "\" from {" << peer->toString() << "}\n";
	} else if (message_type == PEER_NOTIFY_STARTING_TRANSFER) {
		std::string filename;
		sf::Uint32 size;
		packet >> filename >> size;

		File file;
		file.init(filename, size);

		incompleteFiles.push_back(file);

		std::cout << "Beginning download of file: \"" << filename << "\" from {" << peer->toString() << "}\n";
	} else if (message_type == PEER_GIVE_FILE) {
		std::string filename;
		packet >> filename;

		File *file = findIncompleteFile(filename);

		if (file) {
			std::cout << "Downloading file: \"" << filename << "\" Pieces Completed: " << file->getCompletedPieceCount()
					  << " / " << file->getPieceCount() << "  " << file->getCompletionPercentage() * 100 << "%\n";

			file->takeIncoming(packet);
			if (file->isComplete()) {
				std::cout << "File: \"" << filename << "\" download complete. Writing to disk...\n";
				std::cout << "Display file: \"" << filename << "\"\n";
				file->writeToDisk();
				removeIncompleteFile(filename);

				std::cout << "File: \"" << filename << "\" written to disk\n";
			}
		}
	} else {
		std::cout << "Received unknown message type from peer: {" << peer->toString() << "} header: " << message_type
				  << "\n";
	}
}

//handle message from server
void Client::handleServerMessage() {
	sf::Packet packet;
	sf::Socket::Status status = indexServer.socket.receive(packet);
	if (status != sf::Socket::Done) {
		//TODO: do some error stuff
		return;
	}

	sf::Int32 message_type;
	packet >> message_type;
	if (message_type == CLIENT_REQUEST_CONNECTION_INFO) {

		sf::Packet response;
		response << SERVER_GIVE_CONNECTION_INFO;
		response << myIp << listenerPort;

		indexServer.socket.send(response);
		std::cout << "Sent client info to server\n";

	} else if (message_type == CLIENT_NOTIFY_SERVER_SHUTDOWN) {

		waiter.remove(indexServer.socket);
		indexServer.socket.disconnect();
		indexServer.isConnected = false;
		std::cout << "Server disconnected\n";

	} else if (message_type == CLIENT_GIVE_FILE_LOCATION) {
		if (!currentlyTestingServer) {

			std::string filename;
			std::string ip;
			int port;
			packet >> filename >> ip >> port;

			if (ip == "NULL"){
				std::cout << "Server says file not found\n";
				return;
			}

			Connection *peerWithFile = findPeer(ip, port);

			if (!peerWithFile) {
				peerWithFile = connectToPeer(ip, port);
			}

			if (peerWithFile) {
				std::cout << "The file: \"" << filename << "\" is located at {" << peerWithFile->toString() << "}\n";

				sf::Packet message;
				message << PEER_REQUEST_FILE;
				message << filename;

				peerWithFile->socket.send(message);

				std::cout << "Requested file: " << filename << " from {" << peerWithFile->toString() << "}\n";
			}
		}else{
			pendingResponses -= 1;
			if (pendingResponses <= 0) {
				std::cout << "Test took " << timer.getElapsedTime().asMilliseconds() << " ms\n";
				currentlyTestingServer = false;
			}
		}
	} else {
		std::cout << "Received unknown message type from server. header: " << message_type << "\n";
	}
}

//parse cmd input and handle it
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
		handleQuit();
	} else if (commandParts[0] == "connect") {
		handleConnectServer(sf::IpAddress::LocalHost.toString(), serverPort);
	} else if (commandParts[0] == "getfile") {
		if (indexServer.isConnected) {
			sf::Packet message;
			message << SERVER_REQUEST_FILE_LOCATION;
			message << commandParts[1];

			indexServer.socket.send(message);

			std::cout << "Requested file \"" << commandParts[1] << "\"\n";
		} else {
			std::cout << "Error: No server connected\n";
		}
	} else if (commandParts[0] == "addfile") {
		if (indexServer.isConnected) {
			sf::Packet message;
			message << SERVER_REGISTER_FILE;
			message << commandParts[1];

			std::cout << "Registered file \"" << commandParts[1] << "\"\n";

			indexServer.socket.send(message);
		} else {
			std::cout << "Error: No server connected\n";
		}
	} else if (commandParts[0] == "testresponse") { //test server response time

		if (indexServer.isConnected) {
			currentlyTestingServer = true;
			int n = std::stoi(commandParts[1]);
			pendingResponses = n;

			std::cout << "Testing Server Response time with " << n << " queries\n";
			timer.restart();

			lock.unlock();

			for (int i = 0; i < n; ++i) {
				sf::Packet message;
				message << SERVER_REQUEST_FILE_LOCATION;
				message << "testfile";

				indexServer.socket.send(message);
			}

			lock.lock();
		} else {
			std::cout << "Error: No server connected\n";
		}
	} else {
		std::cout << "Sorry, unknown command\n";
	}

	lock.unlock();
}


//this is the loop that waits for messages and passes them to handler functions
//also takes care of new incoming connections
void Client::incomingLoop() {
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
				Connection *newPeer = new Connection();
				listener.accept(newPeer->socket);

				newPeer->ip = newPeer->socket.getRemoteAddress().toString();
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
				handleServerMessage();
			}


		}
		lock.unlock();
	}
}

//send disconnect messages to peers and server, then exit
void Client::handleQuit() {
	sf::Packet message;
	message << SERVER_NOTIFY_CLIENT_DISCONNECT;
	indexServer.socket.send(message);
	waiter.remove(indexServer.socket);
	indexServer.socket.disconnect();

	waiter.remove(listener);
	listener.close();

	message.clear();
	message << PEER_NOTIFY_PEER_DISCONNECT;
	for (int i = 0; i < peers.size(); ++i) {
		peers[i]->socket.send(message);
		waiter.remove(peers[i]->socket);
		peers[i]->socket.disconnect();
		delete peers[i];
	}

	peers.clear();

	timeToExit = true;
}

//try to connect to the server
void Client::handleConnectServer(std::string ip, sf::Uint32 port) {
	std::cout << "Connecting to server...\n";
	if (indexServer.isConnected) {
		waiter.remove(indexServer.socket);
		indexServer.socket.disconnect();
		indexServer.isConnected = false;
	}

	//TODO: error checking
	indexServer.socket.connect(sf::IpAddress(ip), port);
	waiter.add(indexServer.socket);
	indexServer.isConnected = true;

	std::cout << "Connected to server.\n";
}

//search downloading files
File *Client::findIncompleteFile(std::string filename) {
	for (int i = 0; i < incompleteFiles.size(); ++i) {
		if (incompleteFiles[i].filename == filename) {
			return &incompleteFiles[i];
		}
	}

	return nullptr;
}

//remove a pending file, usually because its complete
void Client::removeIncompleteFile(std::string filename) {
	for (int i = 0; i < incompleteFiles.size(); ++i) {
		if (incompleteFiles[i].filename == filename) {
			incompleteFiles.erase(incompleteFiles.begin() + i);
		}
	}
}

//try to connect to a peer
Connection *Client::connectToPeer(std::string ip, sf::Uint32 port) {
	Connection *newPeer = new Connection();
	newPeer->ip = ip;
	newPeer->port = port;

	std::cout << "Attempting to connect to peer {" << newPeer->toString() << "}...\n";

	if (ip == myIp) {
		ip = sf::IpAddress::LocalHost.toString();
	}
	if (newPeer->socket.connect(ip, port) != sf::Socket::Done) {
		std::cout << "Failed to connect to {" << newPeer->toString() << "}\n";
		return nullptr;
	}

	peers.push_back(newPeer);
	waiter.add(newPeer->socket);

	return newPeer;
}


Connection *Client::findPeer(std::string ip, sf::Uint32 port) {
	for (int i = 0; i < peers.size(); ++i) {
		if (peers[i]->ip == ip && peers[i]->port == port) {
			return peers[i];
		}
	}

	return nullptr;
}