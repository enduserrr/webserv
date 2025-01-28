/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:42:49 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/28 18:42:52 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpParser.hpp"
#include <sstream>
#include <iomanip>      //for debugging muotoilu

// Constructor
HttpParser::HttpParser() {}

// Destructor
HttpParser::~HttpParser() {}

// Copy Constructor
// HttpParser::HttpParser(const HttpParser &other) {
//     *this = other;
// }

// // Copy Assignment Operator
// HttpParser& HttpParser::operator=(const HttpParser &other) {
//     if (this != &other) {
//         this->value = other.value;
//     }
//     return *this;
// }


bool HttpParser::parseRequest(std::string& req, size_t max) {
    std::istringstream ss(req);
    std::string line;
    _maxBodySize = max;
    std::getline(ss, line);
    if (!parseStartLine(line)) {
        return false;
    }
    std::string headers; 
    while (getline(ss, line) && line.empty() == 0) {
        headers += line + "\n"; 
    }
    if (!parseHeaders(headers)) {
        return false;
    }
    std::getline(ss, line);
    if (!parseBody(line)) {
        return false;
    }
    return true;
}


bool HttpParser::parseStartLine(std::string& line) {
    std::istringstream ss(line);
    std::string word;

    ss >> word; 
    if (word != "GET" && word != "POST" && word != "DELETE") {
        std::cout << "No method detected!"<< std::endl;
        return false;
    }
    _method = word; 
    ss >> word; 
    if (word[0] != '/') { 
        std::cout << "URI should start with '/'"<< std::endl;
        return false;
    }
    _uri = word; 
    ss >> word;
    if (word != "HTTP/1.1") {
        std::cout << "Wrong HTTP version"<< std::endl;
        return false;
    }
    _httpVersion = word;
    return true;
}

bool HttpParser::parseHeaders(std::string& lines) {
    std::istringstream ss(lines);
    std::string line;
    while(std::getline(ss, line)){
        // std::cout << "parse: " << line << std::endl;
        size_t deli = line.find(':');
        if (deli == std::string::npos) {
            std::cerr << "key + value pair not detected" << std::endl;
            return false;
        }
        std::string key = line.substr(0, deli);
        std::string value = line.substr(deli + 1);
        whiteSpaceTrim(key);
        whiteSpaceTrim(value);
        if (key.empty() || value.empty()) {
            std::cerr << "key or value cannot be empty" << std::endl;
            return false;
        }
        _headers[key] = value;
    }
    return true;
}

bool HttpParser::parseBody(std::string& line) {
    //need more information of what body can be, to make relevant parsing.. :-)
    //for now it is just one line body.. 

    //needs atleast clientbodysize from ServerBlock

    if (line.size() > _maxBodySize) {
        std::cerr << "Too big body (" << line.size() << ")" << std::endl;
        return false;
    }
    _body = line;
    return true; 
}

void HttpParser::whiteSpaceTrim(std::string &str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos) {
        str.clear();
    } else {
        str = str.substr(start, end - start + 1); 
    }
}

// DEBUG 

void HttpParser::display() const {
    
    std::cout << "\n\n-- HTTP-PARSER DISPLAY --\n" << std::endl;
    
    std::cout << "START-LINE" << std::endl;
    std::cout << "method: " << _method << std::endl;
    std::cout << "uri: " << _uri << std::endl;
    std::cout << "http version: " << _httpVersion << std::endl;
    std::cout << "\nHEADERS" << std::endl;
    std::cout << std::left << std::setw(15) << "Key" << "|" << "Value" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        std::cout << std::left << std::setw(15) << it->first << "|" << it->second << std::endl;
    }
    std::cout << "\nBODY\n" << _body << std::endl;
}
