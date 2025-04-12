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

// Constructor
ConfParser::ConfParser(std::string filename) : _fileName(filename) {
    _fileSize = 0;
    _serverBlocks = 0;
}

// Destructor
ConfParser::~ConfParser() {}

//Getters
std::vector<ServerBlock>& ConfParser::getServers() {
    return _servers;
}

//VALIDATE FILE FUNCTIONS

void ConfParser::fileValidation(){
    fileExists();
    filePermissions();
    fileExtension(_fileName, ".conf");
    fileSize();
    Logger::getInstance().logLevel("INFO", "File validation OK", 0);
}

void ConfParser::fileExists(){
    struct stat buffer;
    if (stat(_fileName.c_str(), &buffer) != 0)
        throw std::runtime_error(FILE_V "File does not exist: " + _fileName);
    _fileSize = buffer.st_size;
}

void ConfParser::filePermissions(){
    if (access(_fileName.c_str(), R_OK) != 0)
        throw std::runtime_error(FILE_V "No read permissions: " + _fileName);
}


void ConfParser::fileExtension(const std::string &path, const std::string &ext){
    if (path.size() < ext.size() || path.substr(path.size() - ext.size()) != ext)
        throw std::runtime_error(FILE_V "Invalid file extension: " + path);
}

void ConfParser::fileSize() {
    if (_fileSize > FILE_SIZE_MAX || _fileSize <= FILE_SIZE_MIN)
        throw std::runtime_error(FILE_V "Invalid file size: " + _fileName);
}


// FILE PARSING FUNCTIONS

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

void ConfParser::blocks(int block) {
    if (_serverBlocks == 0)
        throw std::runtime_error(CONF "No server blocks detected");
    if (block != 0)
        throw std::runtime_error(CONF "Unclosed curly brackects");
}

//PARSING DATA FUNCTIONS
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

void ConfParser::keyWordFinder(std::string line, int serverIndex) {
    std::istringstream ss(line);
    std::string key;
    ss >> key;
    if (key == SERVER_NAME)
        parseSingle(ss, _servers[serverIndex], &ServerBlock::setServerName);
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
    else if (key == INDEX)
        parseSingle(ss, _servers[serverIndex], &ServerBlock::setIndex);
    else if (key == ERR_PAGE)
        parseCodeValue(ss, _servers[serverIndex], &ServerBlock::setErrorPage);
    else
        throw std::runtime_error(CONF "unexpected keyword: "+key);
}
#include "Router.hpp"

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
        else if (key == INDEX)
            parseSingle(ss, loc, &Location::setIndex);
        else if (key == USTORE)
            parseSingle(ss, loc, &Location::setUploadStore);
        else if (key == REDIR)
            parseCodeValue(ss, loc, &Location::setRedirect);
        else if (key == ERR_PAGE)
            parseCodeValue(ss, loc, &Location::setErrorPage);
        else
            throw std::runtime_error(CONF "unexpected keyword in location: "+key);
    }
    if (loc.getRoot() == "")
        loc.setRoot(_servers[si].getRoot());
    _servers[si].setLocation(loc);
}

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


//HELPER FUNCTIONS
void whiteSpaceTrim(std::string &str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start == std::string::npos || end == std::string::npos) {
        str.clear();
    } else {
        str = str.substr(start, end - start + 1);
    }
}

std::string removeComments(const std::string &line){
    size_t position = line.find("#");
    if (position != std::string::npos)
        return line.substr(0, position);
    return line;
}
