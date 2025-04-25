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

/**
 * @brief Constructs a Location with default settings.
 */

Location::Location() : _autoIndex(false) {}

/**
 * @brief Default destructor for Location.
 */

Location::~Location() {}

/**
 * @brief Returns the path associated with this location block.
 */

std::string Location::getPath() const {
    return _path;
}

/**
 * @brief Returns the root directory.
 */

const std::string& Location::getRoot()const {
    return _root;
}

/**
 * @brief Returns the upload directory path.
 */

const std::string& Location::getUploadStore() const {
    return _uploadStore;
}

/**
 * @brief Returns whether autoindex is enabled.
 */

bool Location::getAutoIndex() const {
    return _autoIndex;
}

/**
 * @brief Returns the list of allowed HTTP methods.
 */

std::vector<std::string> Location::getAllowedMethods() const {
    return _allowedMethods;
}

/**
 * @brief Returns the map of custom error pages.
 */

const std::map<int, std::string>&  Location::getErrorPages() const {
    return _errorPages;
}

/**
 * @brief Returns the redirection code and URL.
 */

std::pair<int, std::string>  Location::getRedirect() const {
    return _redirect;
}

/**
 * @brief Returns the default index file name for the location.
 */

const std::string& Location::getIndex() const {
    return _index;
}

/**
 * @brief Sets the URL path that maps to this location block.
 */

void Location::setPath(const std::string& path) {
    if (path.empty() || path[0] != '/')
        throw std::runtime_error("Configuration file: Location path must start with '/'");
    _path = path;
}

/**
 * @brief Sets the default index file for the location after validation.
 */

void Location::setIndex(const std::string &name) {
    std::string ext = ".html";
    if (access(("www/" + name).c_str(), R_OK) != 0)
        throw std::runtime_error(CONF "location index file is not valid: " + name);
    if (name.size() < ext.size() || name.substr(name.size() - ext.size()) != ext)
        throw std::runtime_error(CONF "location index file extension: " + name);
    _index = name;
}

/**
 * @brief Checks if the given path points to a valid directory.
 */

static bool isValidDirectory(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

/**
 * @brief Sets the root directory for this location, resolving relative paths.
 */

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

/**
 * @brief Sets the autoindex setting for the location.
 */

void Location::setAutoIndex(const std::string &value) {
    if (value != "on" && value != "off")
        throw std::runtime_error("Autoindex has to be 'on' or 'off'");
    _autoIndex = (value == "on");
}

/**
 * @brief Sets the upload storage path for the location.
 */

void Location::setUploadStore(const std::string &path) {
    _uploadStore = path;
}

/**
 * @brief Adds an allowed HTTP method to the location.
 */

void Location::addAllowedMethod(const std::string& method) {
    _allowedMethods.push_back(method);
}


/**
 * @brief Sets a custom error page path for a specific status code.
 */

void Location::setErrorPage(int code, const std::string &path) {
    if (code < 100 || code > 599)
        throw std::runtime_error("error_page code is invalid: " + std::to_string(code));
    _errorPages[code] = path;
}

/**
 * @brief Sets a redirection response for this location.
 */

void Location::setRedirect(int code, const std::string &url) {
    if (code != 301 && code != 302)
        throw std::runtime_error("redirection code is invalid: " + std::to_string(code));
    _redirect = std::make_pair(code, url);
}
