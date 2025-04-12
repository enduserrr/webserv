/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:43:41 by eleppala          #+#    #+#             */
/*   Updated: 2025/04/01 11:32:37 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Location.hpp"

Location::Location() : _autoIndex(false) {}

Location::~Location() {}

std::string Location::getPath() const {
    return _path;
}

const std::string& Location::getRoot()const {
    return _root;
}

const std::string& Location::getIndex() const {
    return _index;
}

const std::string& Location::getUploadStore() const {
    return _uploadStore;
}

bool Location::getAutoIndex() const {
    return _autoIndex;
}

std::vector<std::string> Location::getAllowedMethods() const {
    return _allowedMethods;
}

const std::map<int, std::string>&  Location::getErrorPages() const {
    return _errorPages;
}

std::pair<int, std::string>  Location::getRedirect() const {
    return _redirect;
}

void Location::setPath(const std::string& path) {
    if (path.empty() || path[0] != '/')
        throw std::runtime_error("Configuration file: Location path must start with '/'");
    _path = path;
}

static bool isValidDirectory(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

void Location::setRoot(const std::string &root) {
    if (isValidDirectory(root)) {
        _root = root; 
        return; 
    }
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd)))
        throw std::runtime_error(CONF "Failed to get current working directory");
    _root = std::string(cwd) + root;
    if (!isValidDirectory(_root))
        throw std::runtime_error(CONF "Invalid root directory: " + _root);
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
