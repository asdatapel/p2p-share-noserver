//
// Created by Asda Tapel on 1/17/17.
//

#ifndef P2P_SHARE_CONST_H
#define P2P_SHARE_CONST_H

const int serverPort = 53002;

//define message types for packet header, some might not be used
const sf::Int32 SERVER_REQUEST_FILE_LOCATION = 0;
const sf::Int32 CLIENT_GIVE_FILE_LOCATION = 1;
const sf::Int32 SERVER_REGISTER_FILE = 2;
const sf::Int32 SERVER_UNREGISTER_FILE = 3;
const sf::Int32 SERVER_UPDATE_FILE = 4;
const sf::Int32 PEER_REQUEST_FILE = 5;
const sf::Int32 PEER_GIVE_FILE = 6;


#endif //P2P_SHARE_CONST_H
