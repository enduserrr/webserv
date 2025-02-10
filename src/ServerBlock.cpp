/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:02 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/10 08:56:25 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ServerBlock.hpp"
#include <iostream>


// Constructor
ServerBlock::ServerBlock() : _autoIndex(false) {}

/* ServerBlock::ServerBlock(ErrorHandler *errorHandler) : _errorHandler(errorHandler) {}

void ServerBlock::setServerName(std::string str) { _serverName = str; }
void ServerBlock::setPort(std::string str) { _port = str; }
void ServerBlock::setBodySize(std::string str) { _bodySize = str; } */

// Destructor
ServerBlock::~ServerBlock() {}

// Getters
std::string ServerBlock::getServerName() const {
    return _serverName;
}

std::string ServerBlock::getRoot() const {
    return _root;
}

std::vector<int> ServerBlock::getPorts() const {
    return _ports;
}

bool ServerBlock::getAutoIndex() {
    return _autoIndex;
}

size_t ServerBlock::getBodySize() const {
    return _bodySize;
}

std::vector<Location>& ServerBlock::getLocations(){
    return _locations;
}


//Setters
void ServerBlock::setServerName(std::string str) {
    _serverName = str;
}

void ServerBlock::setRoot(std::string root) {
    _root = root;
}

void ServerBlock::setPorts(int port) {
    _ports.push_back(port);
}

void ServerBlock::setAutoIndex(bool b) {
    _autoIndex = b;
}

void ServerBlock::setBodySize(size_t size) {
    _bodySize = size;
}

void ServerBlock::setLocation(const Location& loc){
     _locations.push_back(loc);
}

void ServerBlock::setErrorPages(const std::map<int, std::string> &errorPages) {
    ErrorHandler& errorHandler = ErrorHandler::getInstance();  // Get Singleton instance
    for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        errorHandler.setCustomErrorPage(it->first, it->second);
    }
}
