#include "ServerBlock.hpp"


/**
 * @brief Constructs a ServerBlock with default values.
 */

ServerBlock::ServerBlock() : _autoIndex(false), _bodySize(DEFAULT_BODY_SIZE) {}

/**
 * @brief Default destructor for ServerBlock.
 */

ServerBlock::~ServerBlock() {}

/**
 * @brief Returns a reference to the server's root directory path.
 */

std::string& ServerBlock::getRoot() {
    return _root;
}

/**
 * @brief Returns the configured server name.
 */

std::string ServerBlock::getServerName() const {
    return _serverName;
}

/**
 * @brief Returns a copy of the server's configured port list.
 */

std::vector<int> ServerBlock::getPorts(){
    return _ports;
}

/**
 * @brief Returns the autoindex setting for a given location or the server default.
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
 */

size_t ServerBlock::getBodySize() const {
    return _bodySize;
}

/**
 * @brief Returns a reference to the map of all location blocks.
 */

std::map<std::string, Location>& ServerBlock::getLocations() {
    return _locations;
}

/**
 * @brief Retrieves a specific location block by its path key.
 */

Location& ServerBlock::getLocation(const std::string &key) {
    std::map<std::string, Location>::iterator it = _locations.find(key);
    if (it == _locations.end())
        throw std::runtime_error("unknown location key");
    return it->second;
}

/**
 * @brief Returns a reference to the server's error page mappings.
 */

std::map<int, std::string>&  ServerBlock::getErrorPages() {
    return _errorPages;
}


/**
 * @brief Returns the host IP address configured for the server.
 */

const std::string& ServerBlock::getHost() const {
    return _host;
}

/**
 * @brief Sets the host IP address for the server.
 */

void ServerBlock::setHost(const std::string &str) {
    in_addr addr;
    if (inet_pton(AF_INET, str.c_str(), &addr) != 1)
        throw std::runtime_error(CONF "Invalid host format: " + str);
    _host = str;
}

/**
 * @brief Sets the server name after validating for forbidden characters.
 */

void ServerBlock::setServerName(const std::string &str) {
    hasForbiddenSymbols(str);
    _serverName = str;
}

/**
 * @brief Sets the server's root directory path.
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
 */

void ServerBlock::setPort(const std::string &port) {
    int intport = convertToInt(port);
    if (intport < 1 || intport > 65535)
        throw std::runtime_error(CONF "Port has to be in range of 1 - 65535");
    if (std::find(_ports.begin(), _ports.end(), intport) != _ports.end())
        throw std::runtime_error(CONF "Duplicate port");
    _ports.push_back(intport);
}

/**
 * @brief Sets the autoindex setting for the server.
 */

void ServerBlock::setAutoIndex(const std::string &value) {
    if (value != "on" && value != "off")
        throw std::runtime_error(CONF "Autoindex has to be 'on' or 'off'");
    _autoIndex = (value == "on");
}

/**
 * @brief Sets the maximum allowed body size for client requests.
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
 */

void ServerBlock::setLocation(const Location& loc){
     _locations[loc.getPath()] = loc;
}

/**
 * @brief Sets a custom error page path for a specific HTTP status code.
 */

void ServerBlock::setErrorPage(int code, const std::string &path) {
    std::string ext = ".html";
    if (code < 100 || code > 599)
        throw std::runtime_error(CONF "error_page code is invalid: " + std::to_string(code));
    if (access(("www/error_pages/" + path).c_str(), R_OK) != 0)
        throw std::runtime_error(CONF "error_page file is not valid: " + path);
    if (path.size() < ext.size() || path.substr(path.size() - ext.size()) != ext)
        throw std::runtime_error(CONF "error_page invalid file extension: " + path);
    _errorPages[code] = path;
}

/**
 * @brief Checks if the given path is a valid directory.
 */

bool ServerBlock::isValidDirectory(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

/**
 * @brief Converts a numeric string to an integer with validation.
 */

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

/**
 * @brief Validates that a string contains only allowed characters.
 */

void ServerBlock::hasForbiddenSymbols(const std::string &word) {
    for (size_t i = 0; i < word.length(); i++) {
        char c = word[i];
        if (!(std::isalnum(c) || c == '-' || c == '.' || c == '/' || c == '*'))
            throw std::runtime_error(CONF "forbidden symbols in: " + word);
    }
}

/**
 * @brief Checks if the string ends with a valid size unit (k, m, g).
 */

bool ServerBlock::hasValidUnit(const std::string &word) {
    if (word.back() == 'm' || word.back() == 'k' || word.back() == 'g' ||
        word.back() == 'M' || word.back() == 'K' || word.back() == 'G') {
        return true;
    }
    return false;
}
