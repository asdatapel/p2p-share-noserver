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
	File() {
		completePieces = 0;
		totalPieces = 0;
	};

	std::string filename;
	std::size_t size;

	void init(std::string filename, std::size_t size);
	void initFromDisk(std::string filename);
	void send(Connection* peer);
	void takeIncoming(sf::Packet &packet);

	bool isComplete();
	int getPieceCount();
	int getCompletedPieceCount();
	float getCompletionPercentage();

	void writeToDisk();
private:
	std::vector<FilePortion> pieces;

	int completePieces;
	int totalPieces;
};

#endif //P2P_SHARE_FILE_H
