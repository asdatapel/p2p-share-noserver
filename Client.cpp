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