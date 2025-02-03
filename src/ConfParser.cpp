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
#include "Location.hpp"

// Constructor
ConfParser::ConfParser(std::string filename) : _fileName(filename) {
    _fileSize = 0;
    _serverBlocks = 0;
}

// Destructor
ConfParser::~ConfParser() {}

// // Copy Constructor
// ConfParser::ConfParser(const ConfParser &other) {
//     *this = other;
// }

// // Copy Assignment Operator
// ConfParser& ConfParser::operator=(const ConfParser &other) {
//     if (this != &other) {
//         this->value = other.value;
//     }
//     return *this;
// }


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
    else
    {
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
        if (checkServerBlock(line, block)){
            _serverBlocks ++;
        }
        if (!line.empty() && line.find_first_not_of(" \t") != std::string::npos) {
            _fileLines.push_back(line);
        }
    }
    file.close();
    if (!BracketsClosed(block)){
        return false;
    }
    if (!parseData()) {
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

bool ConfParser::checkServerBlock(const std::string &line, int &block) {
    if(block == 0 && line.find(SERVER) != std::string::npos) {
        block ++;
        return true;
    }
    if(line.find("{") != std::string::npos) {
        block ++;
    }
    if(line.find("}") != std::string::npos) {
        block --;
    }
    return false;
}

bool ConfParser::BracketsClosed(int block){
    if (block > 0) {
         std::cerr << "Unclosed curly brackets in configuration File" << std::endl;
        return false;
    }
    return true;
}

bool ConfParser::parseData() {

    int block = 0;
    int serverIndex = -1;

    for (size_t i = 0; i < _fileLines.size(); ++i) {
        if (checkServerBlock(_fileLines[i], block)) {
            std::cout << "serverblock starts.." << std::endl;
            serverIndex ++;
            _servers.push_back(ServerBlock());
        }
        if (block > 0) {
            keyWordFinder(_fileLines[i], serverIndex, i);
        }
    }
    // std::cout << _servers[0].getServerName() << std::endl;
    // std::cout << _servers[1].getServerName() << std::endl;
    // std::cout << _servers[2].getServerName() << std::endl;
    return true;
}

// void ConfParser::keyWordFinder(std::string line, int serverIndex) {

//     std::istringstream ss(line);
//     std:: string word;
//     while(ss >> word) {
//         if (word == SERVER_NAME) {
//             if (ss >> word) {
//                 _servers[serverIndex].setServerName(word);
//             }
//         }
//         else if (word == PORT) {
//             if (ss >> word) {
//                 _servers[serverIndex].setPort(word);
//             }
//         }
//         else if (word == BODY_SIZE) {
//             if (ss >> word) {
//                 _servers[serverIndex].setBodySize(word);
//             }
//         }
//     }
// }

void ConfParser::keyWordFinder(std::string line, int serverIndex, int i) {
    std::istringstream ss(line);
    std::string word;
    while (ss >> word) {
        if (word == SERVER_NAME) {
            if (ss >> word) {
                _servers[serverIndex].setServerName(word);
            }
        } else if (word == PORT) {
            if (ss >> word) {
                _servers[serverIndex].setPort(word);
            }
        } else if (word == BODY_SIZE) {
            if (ss >> word) {
                _servers[serverIndex].setBodySize(convertBodySize(word));
            }
        } else if (word == LOCATION) {
            if (!parseLocation(serverIndex, i)) {
                std::exit(EXIT_FAILURE);
            }
        } else if (word == ERROR_PAGE) {
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
                    std::cerr << "Failed to open error page file: " << pagePath << std::endl;
                }
            }
        }
    }
}

bool ConfParser::parseLocation(int si, int index) {
       
    _servers[si].setLocation(Location());

    for (size_t i = index; i < _fileLines.size(); ++i) {
        if (_fileLines[i].find('}') != std::string::npos){
            break ;
        }
        std::istringstream ss(_fileLines[i]);
        std::string word;
        while(ss >> word){
            if (word == LOCATION) {
                if (ss >> word){
                   _servers[si].getLocations().back().setPath(word); 
                }
            }
            if (word == METHODS) {
                while (ss >> word) {
                    if (!_servers[si].getLocations().back().addAllowedMethod(word)) {
                        std::cerr << "Invalid method: " << word << std::endl;
                        return false; 
                    }
                }
            }
        }
        std::cout << _fileLines[i] << std::endl; 
    }
    return true; 
}


size_t ConfParser::convertBodySize(std::string& word) {
    char unit = '\0';
    if (!word.empty() && word.back() != ';') {
        std::cerr << "Program should stop here: " << std::endl;
        std::cerr << "max_body_size has to end by ';' (fix later to throw exception)" << std::endl;
    }
    if (!word.empty() && word.back() == ';') {
        //after exception fix no need for this if
        //only str_pop; 
        word.pop_back();
    }
    if (!word.empty() && (word.back() == 'm' || word.back() == 'k' || word.back() == 'g')) {
        unit = word.back();
        word.pop_back();
    }
    size_t num = 0;
    try {
        num = std::stoi(word);
    } catch (const std::exception &e) {
        // fix later to for better errorhandling
        std::cerr << "bodysize has to be number" << std::endl;
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



// Debug
void ConfParser::display() {
    std::cout << "\n\nParser information\n" << std::endl;
    std::cout << "File name: " <<_fileName << std::endl;
    std::cout << "File size: "<<_fileSize << std::endl;
    std::cout << "Server blocks: "<<_serverBlocks << std::endl;
    // std::cout << "\nParsed file now: " << std::endl;
    // for (size_t i = 0; i < _fileLines.size(); ++i) {
    //     std::cout << _fileLines[i] << std::endl;
    // }
    std::cout << "\nServer(s): " << std::endl;
    for (size_t i = 0; i < _servers.size(); ++i) {
        std::cout << "server_name:   " << _servers[i].getServerName() << std::endl;
        std::cout << "listen:        " << _servers[i].getPort() << std::endl;
        std::cout << "max_body_size: " << _servers[i].getBodySize() << std::endl;
        std::cout << "location:      " << _servers[i].getLocations()[0].getPath() << std::endl;
        std::cout << "methods:       " << 
        _servers[i].getLocations()[0].getAllowedMethods()[0] << " " << 
        _servers[i].getLocations()[0].getAllowedMethods()[1] << " " << 
        _servers[i].getLocations()[0].getAllowedMethods()[2]<< std::endl;
        // for (size_t j = 0; j < _servers[i].getLocations().size(); ++j) {
        //     std::cout << "Location:      " << _servers[i].getLocations()[j].getPath() << std::endl;

        //     // Print allowed methods
        //     std::cout << "Methods:       ";
        //     for (size_t k = 0; k < _servers[i].getLocations()[j].getAllowedMethods().size(); ++k) {
        //         std::cout << _servers[i].getLocations()[j].getAllowedMethods()[k] << " ";
        //     }
        std::cout << std::endl;
        }
}
