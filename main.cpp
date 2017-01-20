#include <iostream>
#include <thread>
#include <string>

#include <SFML/Network.hpp>

#include "Const.h"

#include "IndexServer.h"
#include "Client.h"

int main() {

#if SERVER == 1
	IndexServer server;
	server.init();

	server.go();
#else
	Client client;
	client.init();

	client.go();
#endif

}