/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:43:41 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/06 09:00:52 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Location.hpp"

// Constructor
Location::Location() : _autoIndex(false) {}

// Destructor
Location::~Location() {}


//getters
std::string Location::getPath() const {
    // std::cout << "getpath: "<< _path << std::endl;
    return _path;
}

std::string& Location::getRoot() {
    return _root;
}

bool& Location::getAutoIndex() {
    return _autoIndex;
}

std::vector<std::string> Location::getAllowedMethods() const {
    return _allowedMethods;
}

std::map<int, std::string>&  Location::getErrorPages() {
    return _errorPages;
}

//setters
void Location::setPath(const std::string& path) {
    _path = path;
}

void Location::setRoot(std::string root) {
    _root = root;
}

void Location::setAutoIndex(bool b) {
    _autoIndex = b;
}

void Location::addAllowedMethod(const std::string& method){
    _allowedMethods.push_back(method);
}

void Location::setErrorPage(int &code, std::string &path) {
    _errorPages[code] = path;
}
