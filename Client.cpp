#include "Client.h"

//just need to clean out the sockets
Client::~Client(){
	for (int i = 0; i < peers.size(); ++i){
		peers[i]->socket.disconnect();
		delete peers[i];
	}
}

void Client::init() {
	listener.listen(serverPort); //random port number
	waiter.add(listener);
}

void Client::go(){
	//TODO: connect to server, then start sending filenames to server

	//TODO: launch a new thread for the incoming message loop, then start another loop for input
}

void Client::incomingLoop(){
	while (waiter.wait()) {    //wait for message to come in or new connection
		lock.lock();

		//check if new connection
		if (waiter.isReady(listener)) {
			Connection *newPeer = new Connection();
			listener.accept(newPeer->socket);

			newPeer->ip = newPeer->socket.getRemoteAddress();
			newPeer->port = newPeer->socket.getRemotePort();

			peers.push_back(newPeer);
			waiter.add(newPeer->socket);
		}

		//check if something from existing connection
		for (int i = 0; i < peers.size(); ++i) {
			if (waiter.isReady(peers[i]->socket)) {
				handleMessage(peers[i]);
			}
		}

		lock.unlock();
	}
}

void Client::handleMessage(Connection *peer) {
	//TODO: handle incoming message, similar to IndexServer::handleMessage
}
