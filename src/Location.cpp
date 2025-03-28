/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:43:41 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/16 12:48:58 by asalo            ###   ########.fr       */
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

const std::string& Location::getIndex() const {
    return _index;
}

const std::string& Location::getUploadStore() const {
    return _uploadStore;
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

std::pair<int, std::string>  Location::getRedirect() const {
    return _redirect;
}

//setters
void Location::setPath(const std::string& path) {
    _path = path;
}

void Location::setRoot(const std::string &root) {
    char cwd[PATH_MAX]; 
    getcwd(cwd, sizeof(cwd)); 
    _root = std::string(cwd) + root;
}

void Location::setAutoIndex(const std::string &value) {
    if (value != "on" && value != "off")
        throw std::runtime_error("Autoindex has to be 'on' or 'off'");
    _autoIndex = (value == "on");
}

void Location::setIndex(const std::string &name) {
    _index = name;
}

void Location::setUploadStore(const std::string &path) {
    _uploadStore = path;
}

void Location::addAllowedMethod(const std::string& method){
    _allowedMethods.push_back(method);
}

void Location::setErrorPage(int code, const std::string &path) {
    if (code < 100 || code > 599)
        throw std::runtime_error("error_page code is invalid: " + std::to_string(code));
    _errorPages[code] = path;
}

void Location::setRedirect(int code, const std::string &url) {
    if (code != 301 && code != 302)
        throw std::runtime_error("redirection code is invalid: " + std::to_string(code));
    _redirect = std::make_pair(code, url);
}
