//
// Created by Asda Tapel on 1/17/17.
//

#include "Client.h"

Client::~Client(){
	for (int i = 0; i < peers.size(); ++i){
		delete peers[i];
	}
}

void Client::init() {
	listener.listen(serverPort); //random port number
	waiter.add(listener);
}