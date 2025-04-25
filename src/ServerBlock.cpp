/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:02 by eleppala          #+#    #+#             */
/*   Updated: 2025/04/01 11:33:26 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ServerBlock.hpp"


/**
 * @brief Constructs a ServerBlock with default values.
 *
 * Sets autoindex to false and body size to the default limit.
 */

ServerBlock::ServerBlock() : _autoIndex(false), _bodySize(DEFAULT_BODY_SIZE) {}


/**
 * @brief Default destructor for ServerBlock.
 */

ServerBlock::~ServerBlock() {}


// std::string ServerBlock::getServerName() const {
//     return _serverName;
// }


/**
 * @brief Returns a reference to the server's root directory path.
 *
 * @return Reference to the root string.
 */

std::string& ServerBlock::getRoot() {
    return _root;
}


/**
 * @brief Returns a copy of the server's configured port list.
 *
 * @return Vector of port numbers.
 */

std::vector<int> ServerBlock::getPorts(){
    return _ports;
}


// const std::string& ServerBlock::getIndex() const {
//     return _index;
// }


/**
 * @brief Returns the autoindex setting for a given location or the server default.
 *
 * If a key is provided and a matching location exists, returns that location's autoindex setting.
 * Otherwise, returns the server-wide autoindex value.
 *
 * @param key Location path to check (optional).
 * @return true if autoindex is enabled, false otherwise.
 */


bool ServerBlock::getAutoIndex(const std::string &key) {
    if (key != "") {
        if (_locations.find(key) != _locations.end())
            return getLocation(key).getAutoIndex();
    }
    return _autoIndex;
}


/**
 * @brief Returns the maximum allowed body size for client requests.
 *
 * @return Body size limit in bytes.
 */

size_t ServerBlock::getBodySize() const {
    return _bodySize;
}


/**
 * @brief Returns a reference to the map of all location blocks.
 *
 * @return Reference to the location map keyed by path.
 */

std::map<std::string, Location>& ServerBlock::getLocations() {
    return _locations;
}


/**
 * @brief Retrieves a specific location block by its path key.
 *
 * @param key The path associated with the desired location block.
 * @return Reference to the matching Location object.
 *
 * @throws std::runtime_error if the key is not found in the location map.
 */

Location& ServerBlock::getLocation(const std::string &key) {
    std::map<std::string, Location>::iterator it = _locations.find(key);
    if (it == _locations.end())
        throw std::runtime_error("unknown location key");
    return it->second;
}


/**
 * @brief Returns a reference to the server's error page mappings.
 *
 * @return Map of HTTP status codes to error page file paths.
 */

std::map<int, std::string>&  ServerBlock::getErrorPages() {
    return _errorPages;
}


/**
 * @brief Returns the host IP address configured for the server.
 *
 * @return Reference to the host string.
 */

const std::string& ServerBlock::getHost() const {
    return _host;
}


// void ServerBlock::setServerName(const std::string &str) {
//     hasForbiddenSymbols(str);
//     _serverName = str;
// }


/**
 * @brief Sets the host IP address for the server.
 *
 * Validates the input as a valid IPv4 address using inet_pton.
 *
 * @param str Host address in dotted-decimal notation.
 * @throws std::runtime_error if the address format is invalid.
 */

void ServerBlock::setHost(const std::string &str) {
    in_addr addr;
    if (inet_pton(AF_INET, str.c_str(), &addr) != 1)
        throw std::runtime_error(CONF "Invalid host format: " + str);
    _host = str;
}


/**
 * @brief Sets the server's root directory path.
 *
 * Appends the provided relative path to the current working directory,
 * validates the resulting path as an existing directory.
 *
 * @param root Relative path to the root directory.
 * @throws std::runtime_error if the current directory can't be retrieved or the final path is invalid.
 */

void ServerBlock::setRoot(const std::string& root) {
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd)))
        throw std::runtime_error(CONF "Failed to get current working directory");
    _root = std::string(cwd) + root;
    if (!isValidDirectory(_root))
        throw std::runtime_error(CONF "Invalid root directory: " + _root);
}


/**
 * @brief Adds a port to the server's list of listening ports.
 *
 * Converts the input to an integer, validates it falls within the allowed range,
 * and ensures the port hasn't been added already.
 *
 * @param port Port number as a string.
 * @throws std::runtime_error if the port is out of range or already defined.
 */

void ServerBlock::setPort(const std::string &port) {
    int intport = convertToInt(port);
    if (intport < 1 || intport > 65535)
        throw std::runtime_error(CONF "Port has to be in range of 1 - 65535");
    if (std::find(_ports.begin(), _ports.end(), intport) != _ports.end())
        throw std::runtime_error(CONF "Duplicate port");
    _ports.push_back(intport);
}

// void ServerBlock::setIndex(const std::string &name) {
//     _index = name;
// }


/**
 * @brief Sets the autoindex setting for the server.
 *
 * Accepts "on" or "off" to enable or disable directory listing.
 *
 * @param value String representing the autoindex state.
 * @throws std::runtime_error if the value is not "on" or "off".
 */

void ServerBlock::setAutoIndex(const std::string &value) {
    if (value != "on" && value != "off")
        throw std::runtime_error(CONF "Autoindex has to be 'on' or 'off'");
    _autoIndex = (value == "on");
}


/**
 * @brief Sets the maximum allowed body size for client requests.
 *
 * Accepts plain numbers or values with units (k, m, g for kilobytes, megabytes, gigabytes).
 * Converts and stores the value in bytes.
 *
 * @param value Body size as a string (e.g., "10m", "5000").
 * @throws std::runtime_error if the value is invalid or negative.
 */

void ServerBlock::setBodySize(const std::string &value) {
    if (hasValidUnit(value)) {
        char unit = value.back();
        std::string newValue = value.substr(0, value.size() - 1);
        int intValue = convertToInt(newValue);
        if (intValue < 0)
            throw std::runtime_error(CONF "Bodysize has to be positive");
        if(unit == 'k' || unit == 'K')
            _bodySize = intValue * 1000;
        if(unit == 'm' || unit == 'M')
            _bodySize = intValue * 1000000;
        if(unit == 'g'|| unit == 'G')
            _bodySize = intValue * 1000000000;

    } else {
        _bodySize = convertToInt(value);
        if (_bodySize < 0)
            throw std::runtime_error(CONF "Bodysize has to be positive");
    }
}


/**
 * @brief Adds or updates a location block in the server.
 *
 * Uses the location's path as the key in the internal map.
 *
 * @param loc Location object to store.
 */

void ServerBlock::setLocation(const Location& loc){
     _locations[loc.getPath()] = loc;
}

void ServerBlock::setErrorPage(int code, const std::string &path) {
    if (code < 100 || code > 599)
        throw std::runtime_error(CONF "error_page code is invalid: " + std::to_string(code));
    _errorPages[code] = path;
}

bool ServerBlock::isValidDirectory(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

int ServerBlock::convertToInt(const std::string &word) {
    if (!std::all_of(word.begin(), word.end(), ::isdigit)) {
        throw std::invalid_argument(CONF "unexpected word: " + word);
    }
    try {
        return stoi(word);
    } catch (const std::exception &e) {
        throw std::invalid_argument(CONF "unexpected word: " + word);
    }
}

void ServerBlock::hasForbiddenSymbols(const std::string &word) {
    for (size_t i = 0; i < word.length(); i++) {
        char c = word[i];
        if (!(std::isalnum(c) || c == '-' || c == '.' || c == '/' || c == '*'))
            throw std::runtime_error(CONF "forbidden symbols in: " + word);
    }
}

bool ServerBlock::hasValidUnit(const std::string &word) {
    if (word.back() == 'm' || word.back() == 'k' || word.back() == 'g' ||
        word.back() == 'M' || word.back() == 'K' || word.back() == 'G') {
        return true;
    }
    return false;
}
