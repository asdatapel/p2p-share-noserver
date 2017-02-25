#include <iostream>
#include <thread>
#include <string>

#include <SFML/Network.hpp>

#include "Const.h"

#include "Client.h"

int main() {

	Client client;
	client.init();

	client.go();

}