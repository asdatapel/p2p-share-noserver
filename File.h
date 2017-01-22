#ifndef P2P_SHARE_FILE_H
#define P2P_SHARE_FILE_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <SFML/Network.hpp>

#include "Const.h"
#include "Connection.h"

#define FILE_PORTION_SIZE 1024
struct FilePortion {
	sf::Int32 portionNumber;
	std::size_t actualSize;
	char data[FILE_PORTION_SIZE];
};

class File {
public:
	File() {};

	std::string filename;
	std::size_t size;

	void init(std::string filename, std::size_t size);
	void initFromDisk(std::string filename);

	void send(Connection* peer);
	void takeIncoming(sf::Packet &packet);

	bool isComplete();

	void writeToDisk();
private:
	std::vector<FilePortion> pieces;

	int remainingPortions;
};

#endif //P2P_SHARE_FILE_H
