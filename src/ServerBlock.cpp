/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:02 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/28 18:44:05 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

std::string& ServerBlock::getRoot() {
    return _root;
}

std::vector<int> ServerBlock::getPorts(){
    return _ports;
}

bool& ServerBlock::getAutoIndex() {
    return _autoIndex;
}

size_t ServerBlock::getBodySize() const {
    return _bodySize;
}

std::vector<Location>& ServerBlock::getLocations() {
    return _locations;
}

std::map<int, std::string>&  ServerBlock::getErrorPages() {
    return _errorPages;
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

void ServerBlock::setBodySize(int size, char unit) {
    if(unit == 'k' || unit == 'K') {
        size = size * 1000; 
    }
    if(unit == 'm' || unit == 'M') {
        size = size * 1000000; 
    }
    if(unit == 'g'|| unit == 'G') {
        size = size * 1000000000; 
    }
    _bodySize = size; 
}

void ServerBlock::setLocation(const Location& loc){
     _locations.push_back(loc);
}

void ServerBlock::setErrorPage(int &code, std::string &path) {
    _errorPages[code] = path;
}
