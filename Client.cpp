#include "Client.h"

Client::Client(int id) {
	myID = id;

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

	readConfigFile();

	listener.listen(listenerPort);
	listenerPort = listener.getLocalPort();
	waiter.add(listener);

	connectToPeers();
}

//begin both threads
void Client::go() {
	std::thread loopThread(&Client::incomingLoop, this);

	inputLoop();

	loopThread.join();
}

void Client::readConfigFile() {
	std::ifstream input("config");

	std::map<int, Connection*> allPeers;

	//read the list of peers with ids
	std::string line;
	while (std::getline(input, line)) {
		std::istringstream ss(line);
		int id, port;
		std::string ip;
		if (!(ss >> id >> ip >> port)) {
			break;
		}
		if (id == -1) {
			break;
		}

		allPeers[id] = new Connection(ip, port);

		if (id == myID){
			listenerPort = port;
		}
	}

	//read all neighbors
	while (std::getline(input, line)) {
		std::istringstream ss(line);
		int id;

		if (!(ss >> id)) {
			break;
		}
		if (id != this->myID) {
			break;
		}

		int peerId;
		while ((ss >> peerId)) {
			peers[peerId] = allPeers[peerId];
		}
	}

	std::cout << "Peer Network Established\n";
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

		for (auto& peer1 : peers) {
			if (peer1.second == peer) {
				peers.erase(peer1.first);
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
	} else if (message_type == PEER_QUERY_HIT) {
		//send message back to original sender about a file being present, provided that message IDs are the same

	} else if (message_type == PEER_QUERY_PEER) {
		//check local index if filename is present
		//if present, send PEER_QUERY_HIT
		//no matter what, forward the filename onto all neighbors

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
	} else if (commandParts[0] == "getfile") {
		/*if (indexServer.isConnected) {
			sf::Packet message;
			message << SERVER_REQUEST_FILE_LOCATION;
			message << commandParts[1];

			indexServer.socket.send(message);

			std::cout << "Requested file \"" << commandParts[1] << "\"\n";
		} else {
			std::cout << "Error: No server connected\n";
		}*/
	} else if (commandParts[0] == "addfile") {
		/*if (indexServer.isConnected) {
			sf::Packet message;
			message << SERVER_REGISTER_FILE;
			message << commandParts[1];

			std::cout << "Registered file \"" << commandParts[1] << "\"\n";

			indexServer.socket.send(message);
		} else {
			std::cout << "Error: No server connected\n";
		}*/
	} else if (commandParts[0] == "testresponse") { //test server response time
		/*
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
		}*/
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

				waiter.add(newPeer->socket);

				replacePeer(newPeer);
				std::cout << "Connected to {" << newPeer->toString() << "}\n";
			}

			//check if something from a connected peer
			for (auto& peer : peers) {
				if (waiter.isReady(peer.second->socket)) {
					handleMessage(peer.second);
				}
			}

		}
		lock.unlock();
	}
}

//send disconnect messages to peers, then exit
void Client::handleQuit() {
	waiter.remove(listener);
	listener.close();

	sf::Packet message;
	message << PEER_NOTIFY_PEER_DISCONNECT;
	for (auto& peer : peers) {
		peer.second->socket.send(message);
		waiter.remove(peer.second->socket);
		peer.second->socket.disconnect();
		delete peer.second;
	}

	peers.clear();

	timeToExit = true;
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
/*Connection *Client::connectToPeer(std::string ip, sf::Uint32 port) {
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
}*/

void Client::connectToPeers() {
	for (auto& peer : peers) {
		std::cout << "Attempting to connect to peer {" << peer.second->toString() << "}...\n";

		if (peer.second->ip == myIp) {
			peer.second->ip = sf::IpAddress::LocalHost.toString();
		}
		if (peer.second->socket.connect(peer.second->ip, peer.second->port) != sf::Socket::Done) {
			std::cout << "Failed to connect to {" << peer.second->toString() << "}\n";
			continue;
		}
		waiter.add(peer.second->socket);
		std::cout << "Connected to {" << peer.second->toString() << "}\n";
	}
}

void Client::replacePeer(Connection *connection){
	for (auto& peer:peers) {
		if (peer.second->ip == connection->ip && peer.second->port == connection->port) {
			delete peers[peer.first];
			peers[peer.first] = connection;
		}
	}
}

Connection *Client::findPeer(std::string ip, sf::Uint32 port) {
	for (auto& peer:peers) {
		if (peer.second->ip == ip && peer.second->port == port) {
			return peer.second;
		}
	}

	return nullptr;
}