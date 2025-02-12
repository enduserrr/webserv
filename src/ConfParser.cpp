/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:43:21 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/11 12:36:00 by asalo            ###   ########.fr       */
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

// Basic checks
bool ConfParser::fileValidation(){
    if (!fileExists())
        std::cout << "File: " << _fileName << " does not exists" << std::endl;
    else if (_fileSize > FILE_SIZE_MAX)
        std::cout << "File: " << _fileName << " is too big" << std::endl;
    else if (_fileSize <= FILE_SIZE_MIN)
        std::cout << "File: " << _fileName << " is empty" << std::endl;
    else if (!filePermissions())
        std::cout << "File: " << _fileName << " deficient permissions" << std::endl;
    else if (!fileExtension())
        std::cout << "File: " << _fileName << " does not end .conf" << std::endl;
    else {
        std::cout << "File validation OK!" << std::endl;
        return true;
    }
    return false;
}

bool ConfParser::fileExists(){
    struct stat buffer;
    if (stat(_fileName.c_str(), &buffer) == 0) {
        _fileSize = buffer.st_size;                  //for handling filesize
        return true;
    }
    return false;
}


bool ConfParser::filePermissions(){
    return (access(_fileName.c_str(), R_OK) == 0); // read permissions only for now.. lets see if more needed
}


bool ConfParser::fileExtension(){
    return _fileName.size() >= 5 && _fileName.substr(_fileName.size() - 5) == ".conf";
}

// Parsing the file


bool ConfParser::checkServerBlock(const std::string &line) {
    if(line != SERVER) {
        return false;
    }
    return true;
}


bool ConfParser::parseFile(){
    std::ifstream file;

    // Open file
    file.open(_fileName.c_str());
    if (!file.is_open()){
        std::cerr << "Failed to open file: " << _fileName << std::endl;
        return false;
    }
    // Rest of the parsing here
    std::string line;
    int         block = 0;
    while (std::getline(file, line)) {
        line = removeComments(line);
        if (line.find_first_not_of(" \t") == std::string::npos) {
            continue ;
        }
        if (checkServerBlock(line)) {
            block ++;
            _serverBlocks ++;
            _fileLines.push_back(line);
            continue ;
        }
        if (!line.empty() && !parseLine(line, block)) {
            file.close();
            return false;
        }
        if (!line.empty() && line.find_first_not_of(" \t") != std::string::npos) {
            _fileLines.push_back(line);
        }
    }
    file.close();
    if (_serverBlocks == 0){
        std::cerr << CONF_ERROR << "no server blocks detected" << std::endl;
        return false;
    }
    if (!BracketsClosed(block)){
        return false;
    }
    if (!parseData()) {
        return false;
    }
    return true;
}

bool ConfParser::parseLine(std::string &line, int &block){
    whiteSpaceTrim(line);
    char last = line.back();
    if (last == '{') {
        block ++;
    }
    if (last == '}') {
        block --;
    }
    if (last != '{' && last != '}' && last != ';') {
        std::cerr << CONF_ERROR << "wrong last char of the line" << std::endl;
        return false;
    }
    if (last == ';'){
        line.pop_back();
    }
    if (line.find(';') != std::string::npos) {
        std::cerr << CONF_ERROR << "';' can be only end of line" << std::endl;
        return false;
    }
    if (line.find('{') != std::string::npos && line.find("location") == std::string::npos) {
        std::cerr << CONF_ERROR << "undefined '{'" << std::endl;
        return false;
    }
    if (line.find('}') != std::string::npos && line.find_first_not_of('}') != std::string::npos){
        std::cerr << CONF_ERROR  << "undefined '}'" << std::endl;
        return false;
    }
    return true;
}



std::string ConfParser::removeComments(const std::string &line){
    size_t position = line.find("#");
    if (position != std::string::npos){
        return line.substr(0, position);
    }
    return line;
}

bool ConfParser::BracketsClosed(int block){
    if (block > 0) {
         std::cerr << "Unclosed curly brackets in configuration File" << std::endl;
        return false;
    }
    return true;
}

int ConfParser::bracketDepth(std::string line){
    if (line.find('{') == std::string::npos){
        return 1;
    }
    if (line.find('}') == std::string::npos){
        return -1;
    }
    return (0);
}

bool ConfParser::parseData() {

    int serverIndex = -1;
    int block = 0;

    for (size_t i = 0; i < _fileLines.size(); ++i) {
        if (_fileLines[i] == "}"){
            block --;
            continue ;
        }
        if (checkServerBlock(_fileLines[i])) {
            _servers.push_back(ServerBlock());
            block ++;
            serverIndex ++;
        }
        else if (_fileLines[i].find(LOCATION) != std::string::npos) {
            if (!parseLocation(serverIndex, i)) {
                return false;
            }
            block --;
        } else {
            keyWordFinder(_fileLines[i], serverIndex, i);
        }
    }
    return true;
}

void ConfParser::keyWordFinder(std::string line, int serverIndex, size_t i) {
    (void)i;
    std::istringstream ss(line);
    std::string word;
    while (ss >> word) {
        std::string temp = word;
        if (temp == SERVER_NAME && ss >> word){
            if (!parseServerName(word, serverIndex)) {
                std::exit(EXIT_FAILURE);
            }
        } else if (temp == PORT && ss >> word) {
            if (!parsePort(word, serverIndex)) {
                std::exit(EXIT_FAILURE);
            }
        } else if (temp == BODY_SIZE && ss >> word) {
                _servers[serverIndex].setBodySize(convertBodySize(word));
        }else if (temp == ROOT && ss >> word) {
            _servers[serverIndex].setRoot(word);
        }else if (temp == AUTOI && ss >> word) {
            _servers[serverIndex].setAutoIndex(true);
            // ServerBlock().setAutoIndex(true);
        } else if (temp == ERROR_PAGE) {
            int statusCode;
            std::string pagePath;
            if (ss >> statusCode >> pagePath) {
                // Read the custom error page
                std::ifstream file(pagePath);
                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    // Store the error page in the temp map
                    _errorPages[statusCode] = buffer.str();
                } else {
                    std::cerr << CONF_ERROR << "Failed to open error page file: " << pagePath << std::endl;
                }
            }
        } else {
            std::cerr << "unknown" << temp <<std::endl;
        }
    }
}

bool ConfParser::validPath(std::string path, int si) {
    whiteSpaceTrim(path);
    if (path.empty() || path[0] != '/') {
        std::cerr << CONF_ERROR << "Path should start with '/'" << std::endl;
        return false;
    }
    if (!_servers[si].getLocations().empty()) {
        for (size_t i = 0; i < _servers[si].getLocations().size(); i ++){
            if (_servers[si].getLocations()[i].getPath() == path) {
                std::cerr << CONF_ERROR << "This server already has this location: " << path << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool ConfParser::parsePort(std::string port, int si){
    if (!wordCheck(port)) {
        return false;
    }
    int num = 0;
    try {
        num = std::stoi(port);
    } catch (const std::exception &e) {
        // fix later to for better errorhandling
        std::cerr << CONF_ERROR << "port has to be number" << std::endl;
        return false;
    }
    if (num < 1 || num > 65535) {
        std::cerr << CONF_ERROR  << "port has to be in range of 1 - 65535" << std::endl;
        return false;
    }
    for (size_t i = 0; i < _servers[si].getPorts().size(); i ++){
        if (num == _servers[si].getPorts()[i]) {
            std::cerr << CONF_ERROR << "Port already exists" << std::endl;
            return false;
        }
    }
    _servers[si].setPorts(num);
    return true;
}

bool ConfParser::wordCheck(std::string &word){
    whiteSpaceTrim(word);
    for (size_t i = 0; i < word.length(); i++) {
        char c = word[i];
        if (!(std::isalnum(c) || c == '-' || c == '.' || c == '/' || c == '*')) {
            std::cerr << CONF_ERROR << "invalid character(s) found" << std::endl;
            return false;
        }
    }
    return true;

}

bool ConfParser::parseServerName(std::string name, int si) {
    if (!wordCheck(name)){
        return false;
    }
    _servers[si].setServerName(name);
    return true;
}

bool ConfParser::parseLocation(int si, size_t &i) {

    Location loc;

    for (; i < _fileLines.size(); ++i) {
        if (_fileLines[i].find('}') != std::string::npos){
            break ;
        }
        std::istringstream ss(_fileLines[i]);
        std::string word;
        while(ss >> word){
            if (word == LOCATION) {
                if (ss >> word){
                    if (!validPath(word, si)) {
                        return false;
                    }
                    loc.setPath(word);
                    if (ss >> word && word != "{"){
                        std::cerr << CONF_ERROR << "extra path detected" << std::endl;
                        return false;
                    }
                }
            }
            if (word == METHODS) {
                while (ss >> word) {
                    if (!loc.addAllowedMethod(word)) {
                        std::cerr << CONF_ERROR << "Invalid method: " << word << std::endl;
                        return false;
                    }
                }
            }
            if (word == ROOT) {
                if (ss >> word) {
                    loc.setRoot(word);
                }
            }
            if (word == AUTOI) {
                if (ss >> word) {
                    loc.setAutoIndex(true);
                }
            }
        }
    }
    _servers[si].setLocation(loc);
    return true;
}

size_t ConfParser::convertBodySize(std::string& word) {
    char unit = '\0';
    if (!word.empty() && (word.back() == 'm' || word.back() == 'k' || word.back() == 'g')) {
        unit = word.back();
        word.pop_back();
    }
    size_t num = 0;
    try {
        num = std::stoi(word);
    } catch (const std::exception &e) {
        // fix later to for better errorhandling
        std::cerr << CONF_ERROR << "bodysize has to be number" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if(unit == 'k') {
        num = num * 1000;
    }
    if(unit == 'm') {
        num = num * 1000000;
    }
    if(unit == 'g') {
        num = num * 1000000000;
    }
    return num;
}

void ConfParser::whiteSpaceTrim(std::string &str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start == std::string::npos || end == std::string::npos) {
        str.clear();
    } else {
        str = str.substr(start, end - start + 1);
    }
}

// Debug
// void ConfParser::display() {
//     std::cout << "\n\nParser information\n" << std::endl;
//     std::cout << "File name: " <<_fileName << std::endl;
//     std::cout << "File size: "<<_fileSize << std::endl;
//     std::cout << "Server blocks: "<<_serverBlocks << std::endl;
//     // std::cout << "\nParsed file now: " << std::endl;
//     // for (size_t i = 0; i < _fileLines.size(); ++i) {
//     //     std::cout << _fileLines[i] << std::endl;
//     // }
//     std::cout << "\nServer(s): " << std::endl;
//     for (size_t i = 0; i < _servers.size(); ++i) {
//         std::cout << "server_name:   " << _servers[i].getServerName() << std::endl;
//         std::cout << "root:          " << _servers[i].getRoot() << std::endl;
//         std::cout << "listen:        ";
//         for (size_t p = 0; p < _servers[i].getPorts().size(); p ++){
//             std::cout << _servers[i].getPorts()[p] << " ";
//         }
//         std::cout << std::endl;
//         std::cout << "max_body_size: " << _servers[i].getBodySize() << std::endl;
//         std::cout << "autoindex:     " << _servers[i].getAutoIndex() << std::endl;
//         std::cout << "location(s)    " << std::endl;
//         for (size_t j = 0; j < _servers[i].getLocations().size(); ++j) {
//             std::cout << "path:          " << _servers[i].getLocations()[j].getPath() << std::endl;
//             std::cout << "root:          " << _servers[i].getLocations()[j].getRoot() << std::endl;
//             std::cout << "autoindex:     " << _servers[i].getLocations()[j].getAutoIndex() << std::endl;
//             // Print allowed methods
//             std::cout << "Methods:       ";
//             for (size_t k = 0; k < _servers[i].getLocations()[j].getAllowedMethods().size(); ++k) {
//                 std::cout << _servers[i].getLocations()[j].getAllowedMethods()[k] << " ";
//             }
//         std::cout << "\n" << std::endl;
//         }
//     }
// }
