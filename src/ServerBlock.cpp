#include "ServerBlock.hpp"

// Constructor
ServerBlock::ServerBlock() {
    std::cout << "SERVERBLOCK constructor" << std::endl;
}

// Destructor
ServerBlock::~ServerBlock() {}

// // Copy Constructor
// ServerBlock::ServerBlock(const ServerBlock &other) {
//     *this = other;
// }

// // Copy Assignment Operator
// ServerBlock& ServerBlock::operator=(const ServerBlock &other) {
//     if (this != &other) {
//         this->value = other.value;
//     }
//     return *this;
// }

// Getters and Setters
std::string ServerBlock::getServerName() const {
    return _serverName;
}

void ServerBlock::setServerName(std::string str) {
    if (!str.empty() && str.back() == ';') {
    str.pop_back();
    }
    _serverName = str; 
}

std::string ServerBlock::getPort() const {
    return _port;
}

void ServerBlock::setPort(std::string str) {
    if (!str.empty() && str.back() == ';') {
    str.pop_back();
    }
    _port = str; 
}

std::string ServerBlock::getBodySize() const {
    return _bodySize;
}

void ServerBlock::setBodySize(std::string str) {
    if (!str.empty() && str.back() == ';') {
    str.pop_back();
    }
    _bodySize = str; 
}


// // Member function
// void ServerBlock::display() const {
//     std::cout << "Value: " << value << std::endl;
// }
