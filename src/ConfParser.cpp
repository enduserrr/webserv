/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:43:21 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/28 18:43:23 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
}

void ConfParser::fileExists(){
    struct stat buffer;
    if (stat(_fileName.c_str(), &buffer) != 0) {
        throw std::runtime_error(ERR FILE_V "File does not exist: " + _fileName);
    }
    _fileSize = buffer.st_size; 
}

void ConfParser::filePermissions(){
    if (access(_fileName.c_str(), R_OK) != 0) {
        throw std::runtime_error(ERR FILE_V "No read permissions: " + _fileName);
    }
}


void ConfParser::fileExtension(const std::string &path, const std::string &ext){
    if (path.size() < ext.size() || path.substr(path.size() - ext.size()) != ext) {
        throw std::runtime_error(ERR FILE_V "Invalid file extension: " + path);
    }
}

void ConfParser::fileSize() {
    if (_fileSize > FILE_SIZE_MAX || _fileSize <= FILE_SIZE_MIN) {
        throw std::runtime_error(ERR FILE_V "Invalid file size: " + _fileName);
    }
}


// FILE PARSING FUNCTIONS
void ConfParser::parseFile(){
    std::ifstream file;
    file.open(_fileName.c_str());
    if (!file.is_open()){
        throw std::runtime_error(ERR "Failed to open file: " + _fileName);
    }
    std::string line;
    int         block = 0;
    while (std::getline(file, line)) {
        line = removeComments(line);
        if (line.find_first_not_of(" \t") == std::string::npos)
            continue ;
        if (line == SERVER) {
            block ++; 
            _serverBlocks ++;
            _fileLines.push_back(line);
            continue ;
        }
        if (!line.empty())
            parseLine(line, block);
        if (!line.empty() && line.find_first_not_of(" \t") != std::string::npos)
            _fileLines.push_back(line);
    }
    file.close();
    blocks(block);
    parseData(); 
}

std::string ConfParser::removeComments(const std::string &line){
    size_t position = line.find("#");
    if (position != std::string::npos){
        return line.substr(0, position);
    }
    return line;
}

void ConfParser::parseLine(std::string &line, int &block){
    whiteSpaceTrim(line);
    char last = line.back();
    if (last == '{')
        block ++; 
    if (last == '}')
        block --; 
    if (last != '{' && last != '}' && last != ';')
        throw std::runtime_error(ERR CONF "line has to end '{', '}' or ';'");
    if (last == ';')
        line.pop_back();
    if (line.find(';') != std::string::npos)
        throw std::runtime_error(ERR CONF "';' can be only end of line");
    if (line.find('{') != std::string::npos && line.find("location") == std::string::npos)
        throw std::runtime_error(ERR CONF "undefined '{'");
    if (line.find('}') != std::string::npos && line.find_first_not_of('}') != std::string::npos)
        throw std::runtime_error(ERR CONF "undefined '}'");
}

void ConfParser::blocks(int block) {
    if (_serverBlocks == 0)
        throw std::runtime_error(ERR CONF "No server blocks detected");
    if (block != 0)
        throw std::runtime_error(ERR CONF "Unclosed curly brackects");
}

//PARSING DATA FUNCTIONS 
void ConfParser::parseData() {
    int serverIndex = -1;
    for (size_t i = 0; i < _fileLines.size(); ++i) {
        if (_fileLines[i] == "}")
            continue ;
        else if (_fileLines[i] == SERVER) {
            _servers.push_back(ServerBlock());
            serverIndex ++;
            continue ;
        }
        else if (_fileLines[i].find(LOCATION) != std::string::npos)
            locationBlock(serverIndex, i);
        else
            keyWordFinder(_fileLines[i], serverIndex);
    }
}

void ConfParser::keyWordFinder(std::string line, int serverIndex) {
    std::istringstream ss(line);
    std::string word;
    ss >> word;
    if (word == SERVER_NAME)
        parseServerName(ss, serverIndex);
    else if (word == PORT)
        parsePort(ss, serverIndex);
    else if (word == BODY_SIZE) 
        parseBodySize(ss, serverIndex);
    else if (word == ROOT)
        parseRoot(ss, _servers[serverIndex].getRoot());
    else if (word == AUTOI)
        parseAutoIndex(ss, _servers[serverIndex].getAutoIndex());
    else if (word == ERR_PAGE)
        parseErrorPages(ss, _servers[serverIndex].getErrorPages()); 
    else 
        throw std::runtime_error(ERR CONF "unexpected keyword: "+word);
}

void ConfParser::parseBodySize(std::istringstream &ss, int si) {
    char unit = '\0';
    std::string word; 
    if (!(ss >> word))
        throw std::runtime_error(ERR CONF);
    if (hasValidUnit(word)) {
        unit = word.back();
        word.pop_back();
    } 
    int num = convertToInt(word, "client_max_body_size");
    if (ss >> word)
        throw std::runtime_error(ERR CONF "unexpected client_max_body_size line");
    _servers[si].setBodySize(num, unit);
}

void ConfParser::parsePort(std::istringstream &ss, int si) {
    std::string word;
    if (!(ss >> word))
        throw std::runtime_error(ERR CONF "unexpected listen line");  
    int port = convertToInt(word, "port");
    if (port < 1 || port > 65535)
        throw std::runtime_error(ERR CONF "port has to be in range of 1 - 65535");
    for (size_t i = 0; i < _servers[si].getPorts().size(); i ++){
        if (port == _servers[si].getPorts()[i])
            throw std::runtime_error(ERR CONF "dublicate port");
    }
    if (ss >> word)
        throw std::runtime_error(ERR CONF "unexpected port line");  
    _servers[si].setPorts(port);
}

void ConfParser::parseServerName(std::istringstream &ss, int si) {
    std::string word; 
    if (!(ss >> word))
        throw std::runtime_error(ERR CONF "unexpected server_name line");
    hasForbiddenSymbols(word);
    _servers[si].setServerName(word);
    if (ss >> word)
        throw std::runtime_error(ERR CONF "unexpected server_name line");
}


//FUNCTIONS FOR BOTH LOCATIONS AND SERVERS
void ConfParser::parseAutoIndex(std::istringstream &ss, bool &autoi) {
    std::string word; 
    if (!(ss >> word) || (word != "on" && word != "off"))
        throw std::runtime_error(ERR CONF "unexpected autoindex line");
    autoi = (word == "on"); 
    if (ss >> word)
        throw std::runtime_error(ERR CONF "unexpected autoindex line"); 
}

void ConfParser::parseRoot(std::istringstream &ss, std::string &root) {
    std::string word; 
    if (!(ss >> word))
        throw std::runtime_error(ERR CONF "unexpected root line");
    //PARSE ROOT - ADD LATER OR ADD root check to setROOT() 
    root = word; 
    if (ss >> word)
        throw std::runtime_error(ERR CONF "unexpected root line");
}

void ConfParser::parseErrorPages(std::istringstream &ss, std::map<int, std::string> &errorPages) {
    std::string key;
    std::string value;
    if (!(ss >> key >> value))
        throw std::runtime_error(ERR CONF "unexpected error_page line");
    int code = convertToInt(key, "error_page");
    errorPages[code] = value; 
    if (ss >> value)
        throw std::runtime_error(ERR CONF "unexpected error_page line");
}


//LOCATION BLOCK FUNCTIONS
void ConfParser::locationBlock(int si, size_t &i) {   
    Location loc;
    for (; i < _fileLines.size(); ++i) {
        if (_fileLines[i].find('}') != std::string::npos)
            break ;
        std::istringstream ss(_fileLines[i]);
        std::string key;
        ss >> key;
        if (key == LOCATION)
            parseLocationLine(ss, loc);
        else if (key == METHODS)
            parseMethods(ss, loc);
        else if (key == ROOT)
            parseRoot(ss, loc.getRoot());
        else if (key == AUTOI)
            parseAutoIndex(ss, loc.getAutoIndex()); 
        else if (key == ERR_PAGE)
            parseErrorPages(ss, loc.getErrorPages());
        else
            throw std::runtime_error(ERR CONF "unexpected keyword in location");
    }
    _servers[si].setLocation(loc);
}

void ConfParser::parseMethods(std::istringstream &ss, Location &loc) {
    std::string word;
    while (ss >> word) {
        if (word != "GET" && word != "POST" && word != "DELETE") 
            throw std::runtime_error(ERR CONF "unexpected allowed_methods line");
        const std::vector<std::string> &exist = loc.getAllowedMethods();
        if (std::find(exist.begin(), exist.end(), word) != exist.end()) {
            throw std::runtime_error(ERR CONF "dublicate method");
        }
        loc.addAllowedMethod(word);
    }
}

void ConfParser::parseLocationLine(std::istringstream &ss, Location &loc) {
    std::string word; 
    if (!(ss >> word))
        throw std::runtime_error(ERR CONF "unexpected location line");
    
    //PARSE PATH - ADD LATER OR ADD path check to setPATH() 
    loc.setPath(word);
    
    if (!(ss >> word))
        throw std::runtime_error(ERR CONF "unexpected location line");
    wordIsExpected(word, "{");
    if (ss >> word)
        throw std::runtime_error(ERR CONF "unexpected location line");
}


//HELPER FUNCTIONS
int ConfParser::convertToInt(std::string &word, const std::string &info) {
    if (!std::all_of(word.begin(), word.end(), ::isdigit)) {
        throw std::invalid_argument(ERR CONF "not a valid " + info + ": " + word); 
    }
    try {
        return stoi(word);
    } catch (const std::exception &e) { 
        throw std::invalid_argument(ERR CONF + info + " conversion failed: " + word);
    }
}

void ConfParser::wordIsExpected(const std::string &word, const std::string &expected) {
    if (word != expected)
        throw std::runtime_error(ERR CONF);
}

void ConfParser::hasForbiddenSymbols(std::string &word) {
    for (size_t i = 0; i < word.length(); i++) {
        char c = word[i];
        if (!(std::isalnum(c) || c == '-' || c == '.' || c == '/' || c == '*'))
            throw std::runtime_error(ERR CONF "forbidden symbols in: " + word);
    }
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

bool ConfParser::hasValidUnit(std::string &word) {
    if (word.back() == 'm' || word.back() == 'k' || word.back() == 'g' ||
        word.back() == 'M' || word.back() == 'K' || word.back() == 'G') {
        return true; 
    }
    return false; 
}


// Debug
void ConfParser::display() {
    std::cout << "\n\nParser information\n" << std::endl;
    std::cout << "File name: " <<_fileName << std::endl;
    std::cout << "File size: "<<_fileSize << std::endl;
    std::cout << "Server blocks: "<<_serverBlocks << std::endl;
    std::cout << "\nServer(s): " << std::endl;
    for (size_t i = 0; i < _servers.size(); ++i) {
        std::cout << "server_name:   " << _servers[i].getServerName() << std::endl;
        std::cout << "root:          " << _servers[i].getRoot() << std::endl; 
        std::cout << "listen:        ";
        for (size_t p = 0; p < _servers[i].getPorts().size(); p ++){
            std::cout << _servers[i].getPorts()[p] << " ";
        } 
        std::cout << std::endl; 
        std::cout << "max_body_size: " << _servers[i].getBodySize() << std::endl;
        std::cout << "autoindex:     " << _servers[i].getAutoIndex() << std::endl;
        std::cout << "error_page(s): " << std::endl; 
        for (std::map<int, std::string>::const_iterator 
            it = _servers[i].getErrorPages().begin(); 
            it != _servers[i].getErrorPages().end(); ++it) {
            std::cout << it->first << "            " << it->second << std::endl;
        }

        std::cout << "\nlocation(s)    " << std::endl;
        for (size_t j = 0; j < _servers[i].getLocations().size(); ++j) {
            std::cout << "path:          " << _servers[i].getLocations()[j].getPath() << std::endl;
            std::cout << "root:          " << _servers[i].getLocations()[j].getRoot() << std::endl;
            std::cout << "autoindex:     " << _servers[i].getLocations()[j].getAutoIndex() << std::endl;
            // Print allowed methods
            std::cout << "Methods:       ";
            for (size_t k = 0; k < _servers[i].getLocations()[j].getAllowedMethods().size(); ++k) {
                std::cout << _servers[i].getLocations()[j].getAllowedMethods()[k] << " ";
            }
            std::cout << "\nerror_page(s): " << std::endl; 
            for (std::map<int, std::string>::const_iterator 
                it = _servers[i].getLocations()[j].getErrorPages().begin(); 
                it != _servers[i].getLocations()[j].getErrorPages().end(); ++it) {
                std::cout << it->first << "            " << it->second << std::endl;
        }

        std::cout << "\n" << std::endl;
        }
    }
}
