#include <iostream>
#include <thread>
#include <string>

#include <SFML/Network.hpp>

#include "Const.h"
#include "IndexServer.h"

void do_server();

int main() {
#ifdef SERVER
	IndexServer server;
	server.init();

	server.go();
#else

#endif
}