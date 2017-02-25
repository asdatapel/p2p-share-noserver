#include <iostream>
#include <thread>
#include <string>

#include <SFML/Network.hpp>

#include "Const.h"

#include "Client.h"

int main(int argc, char *argv[]) {
	int id;
	if (argc < 2){
		id = 0;
	}else {
		id = atoi(argv[1]);
	}

	Client client(id);
	client.init();

	client.go();

}