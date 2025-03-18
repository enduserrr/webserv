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
#include <algorithm>


// Constructor
ServerBlock::ServerBlock() : _autoIndex(false), _bodySize(DEFAULT_BODY_SIZE) {}

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

const std::string& ServerBlock::getIndex() const {
    return _index;
}

bool& ServerBlock::getAutoIndex(const std::string &key) {
    if (key != "") {
        if (_locations.find(key) != _locations.end())
            return getLocation(key).getAutoIndex();
    }
    return _autoIndex;
}

size_t ServerBlock::getBodySize() const {
    return _bodySize;
}

// std::map<std::string, Location>& ServerBlock::getLocations() {
//     return _locations;
// }

Location& ServerBlock::getLocation(const std::string &key) {
    if (_locations.find(key) == _locations.end())
        throw std::runtime_error("unknown key");     
    return _locations[key];
}

std::map<int, std::string>&  ServerBlock::getErrorPages() {
    return _errorPages;
}

//Setters
void ServerBlock::setServerName(const std::string &str) {
    hasForbiddenSymbols(str);
    _serverName = str;
}

void ServerBlock::setRoot(const std::string &root) {
    _root = root;
}

void ServerBlock::setPort(const std::string &port) {
    int intport = convertToInt(port);
    if (intport < 1 || intport > 65535)
        throw std::runtime_error("Port has to be in range of 1 - 65535");
    if (std::find(_ports.begin(), _ports.end(), intport) != _ports.end())
        throw std::runtime_error("Duplicate port");
    _ports.push_back(intport); 
}

void ServerBlock::setIndex(const std::string &name) {
    _index = name;
}

void ServerBlock::setAutoIndex(const std::string &value) {
    if (value != "on" && value != "off")
        throw std::runtime_error("Autoindex has to be 'on' or 'off'");
    _autoIndex = (value == "on");
}

void ServerBlock::setBodySize(const std::string &value) {
    if (hasValidUnit(value)) {
        char unit = value.back();
        std::string newValue = value.substr(0, value.size() - 1);
        int intValue = convertToInt(newValue);
        if (intValue < 0)
            throw std::runtime_error("Bodysize has to be 0 <");
        if(unit == 'k' || unit == 'K') 
            _bodySize = intValue * 1000; 
        if(unit == 'm' || unit == 'M') 
            _bodySize = intValue * 1000000; 
        if(unit == 'g'|| unit == 'G')
            _bodySize = intValue * 1000000000; 
        
    } else {
        _bodySize = convertToInt(value);
        if (_bodySize < 0)
            throw std::runtime_error("Bodysize has to be 0 <");
    }
}

void ServerBlock::setLocation(const Location& loc){
     _locations[loc.getPath()] = loc;
}

void ServerBlock::setErrorPage(int code, const std::string &path) {
    if (code < 100 || code > 599)
        throw std::runtime_error("error_page code is invalid: " + std::to_string(code));
    _errorPages[code] = path;
}


int convertToInt(const std::string &word) {
    if (!std::all_of(word.begin(), word.end(), ::isdigit)) {
        throw std::invalid_argument("unexpected" + word);
    }
    try {
        return stoi(word);
    } catch (const std::exception &e) {
        throw std::invalid_argument("unexpected" + word);
    }
}


void hasForbiddenSymbols(const std::string &word) {
    for (size_t i = 0; i < word.length(); i++) {
        char c = word[i];
        if (!(std::isalnum(c) || c == '-' || c == '.' || c == '/' || c == '*'))
            throw std::runtime_error("forbidden symbols in: " + word);
    }
}

bool hasValidUnit(const std::string &word) {
    if (word.back() == 'm' || word.back() == 'k' || word.back() == 'g' ||
        word.back() == 'M' || word.back() == 'K' || word.back() == 'G') {
        return true;
    }
    return false;
}
