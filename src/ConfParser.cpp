/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:43:21 by eleppala          #+#    #+#             */
/*   Updated: 2025/04/10 14:59:22 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ConfParser.hpp"

/**
 * @brief Constructs a ConfParser with the given configuration file name.
 *
 * Initializes file name, sets file size and server block counters to zero.
 *
 * @param filename Path to the configuration file.
 */

ConfParser::ConfParser(std::string filename) : _fileName(filename) {
    _fileSize = 0;
    _serverBlocks = 0;
}

/**
 * @brief Default destructor for ConfParser.
 */

ConfParser::~ConfParser() {}


/**
 * @brief Returns the list of parsed server blocks.
 *
 * @return Reference to the vector of ServerBlock objects.
 */

std::vector<ServerBlock>& ConfParser::getServers() {
    return _servers;
}


/**
 * @brief Runs all file validation checks on the configuration file.
 *
 * Checks existence, read permissions, extension, and size.
 */

void ConfParser::fileValidation() {
    fileExists();
    filePermissions();
    fileExtension(_fileName, ".conf");
    fileSize();
    Logger::getInstance().logLevel("INFO", "File validation OK", 0);
}


/**
 * @brief Checks if the configuration file exists.
 *
 * Uses stat() to verify file existence and stores its size.
 * Throws if file is missing.
 */

void ConfParser::fileExists() {
    struct stat buffer;
    if (stat(_fileName.c_str(), &buffer) != 0)
        throw std::runtime_error(FILE_V "File does not exist: " + _fileName);
    _fileSize = buffer.st_size;
}


/**
 * @brief Checks if the configuration file has read permissions.
 *
 * Uses access() to confirm read access.
 * Throws if permission is denied.
 */

void ConfParser::filePermissions() {
    if (access(_fileName.c_str(), R_OK) != 0)
        throw std::runtime_error(FILE_V "No read permissions: " + _fileName);
}


/**
 * @brief Validates the file has the correct extension.
 *
 * Ensures the file name ends with the given extension (e.g., ".conf").
 * Throws an error if the extension does not match.
 *
 * @param path Full path or name of the file.
 * @param ext Expected file extension (including dot).
 */

void ConfParser::fileExtension(const std::string &path, const std::string &ext) {
    if (path.size() < ext.size() || path.substr(path.size() - ext.size()) != ext)
        throw std::runtime_error(FILE_V "Invalid file extension: " + path);
}


/**
 * @brief Checks that the configuration file size is within allowed limits.
 *
 * Throws an error if the file is too small or exceeds the maximum size.
 */

void ConfParser::fileSize() {
    if (_fileSize > FILE_SIZE_MAX || _fileSize <= FILE_SIZE_MIN)
        throw std::runtime_error(FILE_V "Invalid file size: " + _fileName);
}


/**
 * @brief Detects the start of a server block in the configuration file.
 *
 * If the line matches the server block start keyword, increments block counters and stores the line.
 * Throws if a server block is already open (invalid structure).
 *
 * @param line Current line being parsed.
 * @param block Reference to current block tracking variable.
 * @return true if the line starts a server block, false otherwise.
 */

bool ConfParser::serverLine(std::string &line, int &block) {
    if (line != SERVER)
        return false;
    if (block != 0)
        throw std::runtime_error(CONF "False serverblock configuration");
    block ++;
    _serverBlocks ++;
    _fileLines.push_back(line);
    return true;
}


/**
 * @brief Performs final validation checks after parsing all configuration data.
 *
 * Ensures each server has a '/' location, at least one port, no duplicate ports,
 * and that root values are set at both server and location levels.
 *
 * @throws std::runtime_error if any required configuration is missing or invalid.
 */

void ConfParser::allSetted() {
    std::vector<int> allPorts; 
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i].getLocations().count("/") == 0)
            throw std::runtime_error(CONF "Missing '/' location block");
        if (_servers[i].getPorts().empty())
            throw std::runtime_error(CONF "You need to set atleast one port per server");
        const std::vector<int>& ports = _servers[i].getPorts();
        for (size_t j = 0; j < ports.size(); ++j) {
            if (std::find(allPorts.begin(), allPorts.end(), ports[j]) != allPorts.end())
                throw std::runtime_error(CONF "Duplicate port used in multiple servers");
            allPorts.push_back(ports[j]);
        }
        if (_servers[i].getRoot().empty())
            throw std::runtime_error(CONF "Root not setted (Global level)");
        for (std::map<std::string, Location>::iterator it = _servers[i].getLocations().begin();
                it != _servers[i].getLocations().end(); ++it) {
            if (it->second.getRoot().empty())
                throw std::runtime_error(CONF "Root not setted (Location level)");
        }
    }
}


/**
 * @brief Parses the configuration file line by line.
 *
 * Opens the file, removes comments, trims whitespace, and processes each line.
 * Detects server blocks, validates configuration structure, and stores clean lines.
 * Calls further parsing and validation routines after reading the file.
 *
 * @throws std::runtime_error if the file can't be opened or the structure is invalid.
 */

void ConfParser::parseFile() {
    std::ifstream file(_fileName);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + _fileName);
    std::string line;
    bool hasContent = false;
    int block = 0;
    while (std::getline(file, line)) {
        line = removeComments(line);
        if (line.find_first_not_of(" \t") == std::string::npos)
            continue ;
        whiteSpaceTrim(line);
        if (serverLine(line, block)){
            hasContent = false;
            continue;
        }
        if (block < 1 || block > 2)
            throw std::runtime_error(CONF "False serverblock configuration");
        if (!line.empty())
            parseLine(line, block, hasContent);
        if (!line.empty() && line.find_first_not_of(" \t") != std::string::npos) {
            _fileLines.push_back(line);
        }
    }
    blocks(block);
    parseData();
    allSetted();
    Logger::getInstance().logLevel("INFO", "Server configurations OK", 0);
}


/**
 * @brief Validates the ending character of a configuration line and updates block state.
 *
 * Handles opening/closing of blocks with '{' and '}', validates semicolon usage,
 * and checks for correct 'location' block syntax.
 *
 * @param line Line to validate and modify.
 * @param block Current block level, incremented or decremented as needed.
 * @param hasContent Indicates whether the current block contains valid content.
 *
 * @throws std::runtime_error on any structural or syntax error.
 */

void parseLast(std::string &line, int &block, bool hasContent) {
    char last = line.back();
    if (last == '{') {
        line.pop_back();
        block ++;
    }
    if (last == '}') {
        if (!hasContent)
            throw std::runtime_error(CONF "Empty block(s) in configuration");
        block --;
    }
    if (last != '{' && last != '}' && last != ';')
        throw std::runtime_error(CONF "line has to end '{', '}' or ';'");
    if (last == ';' && line.find_first_not_of(";}") != std::string::npos)
        line.pop_back();
    if (line.find(LOCATION) != std::string::npos && last != ('{'))
        throw std::runtime_error(CONF "False usage of location");
}


/**
 * @brief Parses and validates a single configuration line.
 *
 * Trims the line, checks for unexpected characters (;, {, }), and delegates content parsing.
 * Updates block content status via the hasContent flag.
 *
 * @param line Line to parse and validate.
 * @param block Current block type indicator.
 * @param hasContent Reference to content flag indicating valid config presence.
 */

void ConfParser::parseLine(std::string &line, int &block, bool &hasContent){
    whiteSpaceTrim(line);
    parseLast(line, block, hasContent);
    if (line.find(';') != std::string::npos)
        throw std::runtime_error(CONF "undefined ';'");
    if (line.find('{') != std::string::npos)
        throw std::runtime_error(CONF "undefined '{'");
    if (line.find('}') != std::string::npos && line.find_first_not_of('}') != std::string::npos)
        throw std::runtime_error(CONF "undefined '}'");
    hasContent = (line.find(LOCATION) == std::string::npos);
}


/**
 * @brief Validates overall block structure after parsing.
 *
 * Ensures that at least one server block was found and all opened blocks are properly closed.
 *
 * @param block Final block level counter after parsing.
 * @throws std::runtime_error if structure is incomplete or invalid.
 */

void ConfParser::blocks(int block) {
    if (_serverBlocks == 0)
        throw std::runtime_error(CONF "No server blocks detected");
    if (block != 0)
        throw std::runtime_error(CONF "Unclosed curly brackects");
}


/**
 * @brief Parses cleaned configuration lines into server and location blocks.
 *
 * Iterates through all stored lines, identifies server and location contexts,
 * and delegates parsing to appropriate functions.
 *
 * @throws std::runtime_error if configuration structure is invalid.
 */

void ConfParser::parseData() {
    int serverIndex = -1;
    int serverLevel = 0;
    for (size_t i = 0; i < _fileLines.size(); ++i) {
        if (_fileLines[i] == "}") {
            continue ;
        }
        else if (_fileLines[i] == SERVER) {
            _servers.push_back(ServerBlock());
            serverLevel = 0;
            serverIndex ++;
            continue ;
        }
        else if (_fileLines[i].find(LOCATION) != std::string::npos) {
            serverLevel = 1;
            locationBlock(serverIndex, i);
        }
        else if (serverLevel == 0)
            keyWordFinder(_fileLines[i], serverIndex);
        else
            throw std::runtime_error(CONF "False configuration");
    }
}


/**
 * @brief Parses and applies a single server-level directive based on the keyword.
 *
 * Reads the keyword from the line and dispatches to the appropriate setter on the ServerBlock.
 * Supports both single-value and code-value directives.
 *
 * @param line Configuration line containing the directive.
 * @param serverIndex Index of the current ServerBlock in the server list.
 *
 * @throws std::runtime_error if the keyword is unrecognized.
 */

void ConfParser::keyWordFinder(std::string line, int serverIndex) {
    std::istringstream ss(line);
    std::string key;
    ss >> key;
    // if (key == SERVER_NAME)
    //     parseSingle(ss, _servers[serverIndex], &ServerBlock::setServerName);
    if (key == HOST)
        parseSingle(ss, _servers[serverIndex], &ServerBlock::setHost);
    else if (key == PORT)
        parseSingle(ss, _servers[serverIndex], &ServerBlock::setPort);
    else if (key == BODY_SIZE)
        parseSingle(ss, _servers[serverIndex], &ServerBlock::setBodySize);
    else if (key == ROOT)
        parseSingle(ss, _servers[serverIndex], &ServerBlock::setRoot);
    else if (key == AUTOI)
        parseSingle(ss, _servers[serverIndex], &ServerBlock::setAutoIndex);
    // else if (key == INDEX)
    //     parseSingle(ss, _servers[serverIndex], &ServerBlock::setIndex);
    else if (key == ERR_PAGE)
        parseCodeValue(ss, _servers[serverIndex], &ServerBlock::setErrorPage);
    else
        throw std::runtime_error(CONF "unexpected keyword: "+key);
}


/**
 * @brief Sets default values for a location block if certain fields are unset.
 *
 * Inherits the root from the server if not explicitly set.
 * Adds GET, POST, and DELETE as default allowed methods if none are defined.
 *
 * @param si Index of the parent server in the server list.
 * @param loc Location object to update.
 */

void ConfParser::locationFallbacks(int si, Location &loc) {
    if (loc.getRoot() == "")
        loc.setRoot(_servers[si].getRoot());
    if (loc.getAllowedMethods().empty()) {
        loc.addAllowedMethod("GET");
        loc.addAllowedMethod("POST");
        loc.addAllowedMethod("DELETE");
    }
}


/**
 * @brief Parses a location block and sets its values into the corresponding server.
 *
 * Reads configuration lines starting from a 'location' block, identifies and parses each directive,
 * and sets the parsed Location object to the specified server.
 *
 * @param si Index of the current server in the server list.
 * @param i Reference to the current line index (will be incremented inside the loop).
 *
 * @throws std::runtime_error if an unknown keyword is found or the block structure is invalid.
 */

void ConfParser::locationBlock(int si, size_t &i) {
    Location loc;
    for (; i < _fileLines.size(); ++i) {
        std::istringstream ss(_fileLines[i]);
        std::string key;
        ss >> key;
        if (key == "}")
            break ;
        else if (key == LOCATION)
            parseSingle(ss, loc, &Location::setPath);
        else if (key == METHODS)
            parseMethods(ss, loc);
        else if (key == ROOT)
            parseSingle(ss, loc, &Location::setRoot);
        else if (key == AUTOI)
            parseSingle(ss, loc, &Location::setAutoIndex);
        // else if (key == INDEX)
        //     parseSingle(ss, loc, &Location::setIndex);
        else if (key == USTORE)
            parseSingle(ss, loc, &Location::setUploadStore);
        else if (key == REDIR)
            parseCodeValue(ss, loc, &Location::setRedirect);
        else if (key == ERR_PAGE)
            parseCodeValue(ss, loc, &Location::setErrorPage);
        else
            throw std::runtime_error(CONF "unexpected keyword in location: "+key);
    }
    locationFallbacks(si, loc); 
    _servers[si].setLocation(loc);
}


/**
 * @brief Parses a numeric code and associated value, then applies them using the provided setter.
 *
 * Reads an integer and a string from the stream and sets them on the object via the given setter.
 * Throws if parsing fails or if extra tokens are present.
 *
 * @tparam T Type of the object being modified.
 * @param ss Input stream containing the code and value.
 * @param obj Object to modify.
 * @param setter Member function pointer taking (int, std::string).
 *
 * @throws std::runtime_error on format errors or unexpected extra input.
 */

template <typename T>
void ConfParser::parseCodeValue(std::istringstream &ss, T &obj, void (T::*setter)(int, const std::string&)) {
    int code;
    std::string url;
    if (!(ss >> code) || !(ss >> url))
        throw std::runtime_error(CONF "unexpected line");
    (obj.*setter)(code, url);
    if (ss >> url)
        throw std::runtime_error(CONF "unexpected word: " + url);
}


/**
 * @brief Parses a single-word directive and applies it using the provided setter.
 *
 * Extracts one word from the stream and passes it to the object's setter function.
 * Throws if the line contains more than one word.
 *
 * @tparam T Type of the object being modified.
 * @param ss Input stream containing the configuration line.
 * @param obj Object to modify.
 * @param setter Member function pointer used to set the value.
 *
 * @throws std::runtime_error if parsing fails or extra tokens are found.
 */

template <typename T>
void ConfParser::parseSingle(std::istringstream &ss, T &obj, void (T::*setter)(const std::string&)) {
    std::string word;

    if (!(ss >> word))
        throw std::runtime_error(CONF "unexpected line");
    (obj.*setter)(word);
    if ((ss >> word)) {
        throw std::runtime_error(CONF "unexpected word: " + word);
    }
}


/**
 * @brief Parses and sets allowed HTTP methods for a location block.
 *
 * Reads method names from the stream, checks for validity and duplicates,
 * and adds them to the Location object.
 *
 * @param ss Input stream containing method names.
 * @param loc Location object to update.
 *
 * @throws std::runtime_error if an invalid or duplicate method is found.
 */

void ConfParser::parseMethods(std::istringstream &ss, Location &loc) {
    std::string word;
    while (ss >> word) {
        if (word != "GET" && word != "POST" && word != "DELETE")
            throw std::runtime_error(CONF "unexpected line");
        const std::vector<std::string> &exist = loc.getAllowedMethods();
        if (std::find(exist.begin(), exist.end(), word) != exist.end())
            throw std::runtime_error(CONF "dublicate method");
        loc.addAllowedMethod(word);
    }
}


/**
 * @brief Trims leading and trailing whitespace (spaces and tabs) from a string.
 *
 * If the string contains only whitespace, clears it entirely.
 *
 * @param str String to trim (modified in-place).
 */

void whiteSpaceTrim(std::string &str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start == std::string::npos || end == std::string::npos) {
        str.clear();
    } else {
        str = str.substr(start, end - start + 1);
    }
}


/**
 * @brief Removes comments from a line.
 *
 * Strips everything after the first '#' character, if present.
 *
 * @param line Input line from the configuration file.
 * @return Line without comments.
 */

std::string removeComments(const std::string &line){
    size_t position = line.find("#");
    if (position != std::string::npos)
        return line.substr(0, position);
    return line;
}
