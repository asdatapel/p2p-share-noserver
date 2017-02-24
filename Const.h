#ifndef P2P_SHARE_CONST_H
#define P2P_SHARE_CONST_H

const int serverPort = 53002; //random port number, if you build and rebuild quickly, you should change

/* define message types for packet header, some might not be used
 * this is how we'll know kind of data each packet contains */
const sf::Int32 NULL_MESSAGE = 0;

const sf::Int32 SERVER_GIVE_CONNECTION_INFO = 1;
const sf::Int32 SERVER_NOTIFY_CLIENT_DISCONNECT = 2;
const sf::Int32 SERVER_REGISTER_FILE = 3;
const sf::Int32 SERVER_UNREGISTER_FILE = 4;
const sf::Int32 SERVER_REQUEST_FILE_LOCATION = 5;
const sf::Int32 SERVER_UPDATE_FILE = 6;

const sf::Int32 CLIENT_REQUEST_CONNECTION_INFO = 7;
const sf::Int32 CLIENT_NOTIFY_SERVER_SHUTDOWN = 8;
const sf::Int32 CLIENT_GIVE_FILE_LOCATION = 9;

const sf::Int32 PEER_NOTIFY_PEER_DISCONNECT = 10;
const sf::Int32 PEER_REQUEST_FILE = 11;                 //OBTAIN
const sf::Int32 PEER_NOTIFY_STARTING_TRANSFER = 12;
const sf::Int32 PEER_GIVE_FILE = 13;                    //DOWNLOAD

const sf::Int32 PEER_QUERY_HIT = 14;
const sf::Int32 PEER_QUERY_PEER = 15;

#endif //P2P_SHARE_CONST_H
