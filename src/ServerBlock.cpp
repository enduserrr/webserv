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
ServerBlock::ServerBlock() {
    std::cout << "SERVERBLOCK constructor" << std::endl;
}

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

std::vector<int> ServerBlock::getPorts() {
    return _ports;
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

void ServerBlock::setPorts(int port) {
    _ports.push_back(port); 
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

/* // Member function
void ServerBlock::display() const {
    std::cout << "Value: " << value << std::endl;
} */
