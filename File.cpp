#include "File.h"

sf::Packet &operator<<(sf::Packet &packet, const FilePortion &portion);
sf::Packet &operator>>(sf::Packet &packet, FilePortion &portion);

void File::init(std::string filename, std::size_t size) {
	this->filename = filename;
	this->size = size;

	totalPieces = (int) (size / FILE_PORTION_SIZE) + 1;

	for (int i = 0; i < totalPieces; ++i) {
		pieces.push_back(FilePortion());
	}
}

void File::initFromDisk(std::string filename) {
	this->filename = filename;

	std::ifstream stream;
	stream.open(filename, std::ios::in | std::ios::binary);

	struct stat stats;
	if (!stat(filename.c_str(), &stats)) {
		//TODO: error stuff
	}

	size = stats.st_size;

	totalPieces = (int) (size / FILE_PORTION_SIZE) + 1;

	for (int i = 0; i < totalPieces; ++i) {
		pieces.push_back(FilePortion());
		stream.read(pieces[i].data, FILE_PORTION_SIZE);
		pieces[i].portionNumber = i;
		pieces[i].actualSize = stream.gcount();
	}

}

void File::send(Connection *peer) {
	sf::Packet packet;
	for (int i = 0; i < pieces.size(); ++i) {
		packet.clear();
		packet << PEER_GIVE_FILE;
		packet << filename << pieces[i].portionNumber << (sf::Uint32)pieces[i].actualSize;
		packet << pieces[i];

		peer->socket.send(packet);
	}
}

void File::takeIncoming(sf::Packet &packet) {
	sf::Int32 portionNumber;
	packet >> portionNumber;

	sf::Uint32 size;
	packet >> size;
	pieces[portionNumber].actualSize = size;

	packet >> pieces[portionNumber];

	++completePieces;
}

float File::getCompletionPercentage(){
	return (float)completePieces / (float) totalPieces;
}

int File::getPieceCount(){
	return totalPieces;
}

int File::getCompletedPieceCount(){
	return completePieces;
}

bool File::isComplete() {
	return ((totalPieces - completePieces) == 0);
}

void File::writeToDisk() {
	std::ofstream stream;
	stream.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

	for (int i = 0; i < pieces.size(); ++i){
		stream.write(pieces[i].data, pieces[i].actualSize);
	}
}


sf::Packet &operator<<(sf::Packet &packet, const FilePortion &portion) {
	packet.append(portion.data, portion.actualSize);

	return packet;
}

sf::Packet &operator>>(sf::Packet &packet, FilePortion &portion) {
	std::size_t offset1 = sizeof(sf::Int32);
	sf::Uint32 nameLength = ntohl(*reinterpret_cast<const sf::Uint32*>((const char *)(packet.getData()) + offset1));;

	std::size_t offset = offset1 + sizeof(sf::Uint32) + nameLength + sizeof(sf::Int32) + sizeof(sf::Uint32);
	const void *data = (const char *) (packet.getData()) + offset;
	std::memcpy(portion.data, data, portion.actualSize);

	return packet;
}