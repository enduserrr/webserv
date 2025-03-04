/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:42:49 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/02 18:55:59 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpParser.hpp"
#include "Types.hpp"
#include <sstream>
#include <iomanip>

/* Constructor */
HttpParser::HttpParser() {}

/* Destructor */
HttpParser::~HttpParser() {}

bool HttpParser::isFullRequest(std::string &input) {
    size_t headerEnd = input.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;
    size_t bodyStart = headerEnd + 4;
    size_t contentLengthPos = input.find("Content-Length:");
    if (contentLengthPos != std::string::npos) {
        size_t contentLengthEnd = input.find("\r\n", contentLengthPos);
        if (contentLengthEnd == std::string::npos)
            return false;
        std::string contentLengthValue = input.substr(contentLengthPos + 15, contentLengthEnd - (contentLengthPos + 15));
        int contentLength = std::stoi(contentLengthValue);
        if (input.size() < bodyStart + contentLength)
            return false;
    }
    _fullRequest = input.substr(0, bodyStart + contentLengthPos);
    std::cout << "\nONE FULL REQUEST:" << _fullRequest << std::endl; 
    input = input.substr(bodyStart);
    std::cout << "\n\nLEFT OVER:" << input << std::endl; 
    return true;
}

bool HttpParser::parseRequest(ServerBlock &block) {
    (void)block;

    std::istringstream ss(_fullRequest);
    std::string line;
    _maxBodySize = block.getBodySize();
    HttpRequest request;
    std::getline(ss, line);
    try {
        parseStartLine(line, request);
        while (getline(ss, line, '\r') && ss.get() == '\n' && !line.empty()){
            parseHeader(line, request);
        }
        std::string body;
        body.assign(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>());
        parseBody(body, request);
        createRequest(block, request);
    } catch (...) {
        std::cout <<"false"<<std::endl;
        return false;
    }
    std::cout <<"true"<<std::endl;
    return true;
}

void HttpParser::parseVersion(std::istringstream &ss, HttpRequest &req){
    std::string word;
    if (!(ss >> word))
        throw std::runtime_error("Silent");
    if (word != "HTTP/1.1")
        throw std::runtime_error("Silent");
    req.setHttpVersion(word);
}

//STARTLINE SYNTAX: [method] [URI] [HTTP_VERSION]
void HttpParser::parseStartLine(std::string &line, HttpRequest &req) {
    std::istringstream ss(line);
    parseMethod(ss, req);
    parseUri(ss, req);
    parseVersion(ss, req);
}

void HttpParser::parseMethod(std::istringstream &ss, HttpRequest &req) {
    std::string word;
    if (!(ss >> word))
        throw std::runtime_error("Silent");
    if (word != "GET" && word != "POST" && word != "DELETE")
        throw std::runtime_error("Silent");
    req.setMethod(word);
}

//valid syntax: /dir/subdir?query=string  -- /dir/subdir?color=red&size=large
void HttpParser::parseUri(std::istringstream &ss, HttpRequest &req) {
    std::string word;
    std::string temp;
    if (!(ss >> word))
        throw std::runtime_error("Silent");
    for (size_t i = 0; i < word.length(); i++) {
        if(word[i] == '?')
            parseUriQuery(word.substr(i + 1), req);
        temp += word[i];
    }
    isValidUri(temp);
    req.setUri(temp);
}

void HttpParser::parseUriQuery(const std::string &query, HttpRequest &req) {
    if (query.empty())
        throw std::runtime_error("Silent");
    std::stringstream ss(query);
    std::string token;
    while (std::getline(ss, token, '&')) {
        if (token.empty())
            throw std::runtime_error("Silent");
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            if (key.empty())
                throw std::runtime_error("Silent");
            req.setUriQuery(key, value);
        } else {
            req.setUriQuery(token, "true");
        }
    }
}

void HttpParser::isValidUri(std::string& uri) {
    if (uri.empty() || uri[0] != '/')
        throw std::runtime_error("Silent");
    std::string invalidChars = "<>{}|\\^`\" ";
    for (size_t i = 0; i < uri.length(); i++) {
        if (invalidChars.find(uri[i]) != std::string::npos)
            throw std::runtime_error("Silent");
    }
}

void HttpParser::parseHeader(std::string &line, HttpRequest &req) {
    std::istringstream ss(line);
    std::string key;
    std::string value;
    if (!(ss >> key >> value))
        throw std::runtime_error("Silent");
    key.pop_back();
    if (key == "Content-Length") {
        size_t size = std::stoi(value);
        if (size > _maxBodySize)
            throw std::runtime_error("413 Payload Too Large");
    }
    req.addNewHeader(key, value);
}

void HttpParser::parseBody(std::string &body, HttpRequest &req) {
    Types types; // Use the centralized Types class
    std::string contentType = req.getHeader("Content-Type");
    std::cout << RB << contentType << RES << std::endl;
    std::string emptyBody = "";

    if (contentType.empty()) {
        // std::cerr << "Empty Body" << std::endl;
        req.setBody(emptyBody);
        return;
    }

    if (contentType == "application/x-www-form-urlencoded") {
        req.setBody(body);
    }
    else if (contentType.find("multipart/form-data") == 0) {
        size_t boundaryPos = contentType.find("boundary=");
        if (boundaryPos != std::string::npos) {
            std::string boundary = "--" + contentType.substr(boundaryPos + 9);
            std::cout << "Extracted boundary: " << boundary << std::endl;
        } else {
            std::cerr << "Error: Missing boundary in Content-Type: " << contentType << std::endl;
        }

        std::string boundary = "--" + contentType.substr(boundaryPos + 9);
        size_t dispositionPos = body.find("Content-Disposition: form-data;");
        if (dispositionPos == std::string::npos) {
            std::cerr << "Error: Missing Content-Disposition header" << std::endl;
            req.setBody(emptyBody);
            return;
        }

        // Extract filename
        size_t filenamePos = body.find("filename=\"", dispositionPos);
        if (filenamePos != std::string::npos) {
            filenamePos += 10; // Move past "filename=\""
            size_t endPos = body.find("\"", filenamePos);
            if (endPos != std::string::npos) {
                std::string filename = body.substr(filenamePos, endPos - filenamePos);
                req.setFileName(filename); // Store filename
            }
        }

        // Extract file content
        size_t contentStart = body.find("\r\n\r\n", dispositionPos);
        if (contentStart != std::string::npos) {
            contentStart += 4; // Move past header section
            size_t contentEnd = body.find(boundary, contentStart);
            if (contentEnd != std::string::npos) {
                std::string value = body.substr(contentStart, contentEnd - contentStart - 2);
                req.setBody(value);
            } else {
                std::string value = body.substr(contentStart);
                req.setBody(value);
            }
        }
    }
    else if (types.isValidContent(contentType)) {
        req.setBody(body);
    }
    else {
        std::cerr << "Error: Unsupported Content-Type: " << contentType << std::endl;
        req.setBody(emptyBody);
    }
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

bool HttpParser::createRequest(ServerBlock &block, HttpRequest &req) {
    bool matched = false;
    std::vector<Location>& locations = block.getLocations();
    for (size_t i = 0; i < locations.size(); i++) {
        // Use req.getUri() to check the request's URI against each location's path.
        if (req.getUri().find(locations[i].getPath()) == 0) {
            req.setAutoIndex(locations[i].getAutoIndex());
            req.setRoot(locations[i].getRoot());
            matched = true;
            break;
        }
    }
    // If no matching location, use the ServerBlock's global settings.
    if (!matched) {
        req.setAutoIndex(block.getAutoIndex());
        req.setRoot(block.getRoot());
    }
    _requests.push_back(req);
    return true;
}

/* Return all requests */
std::vector<HttpRequest>& HttpParser::getRequests() {
    return _requests;
}

/* Return one request (FIFO) */
HttpRequest& HttpParser::getPendingRequest() {
    return _requests.front();
}

void HttpParser::removeRequest() {
    if (!_requests.empty()) {
        _requests.erase(_requests.begin());
    }
}

/* DEBUG */
void HttpParser::display() const {

    std::cout << "\n\n-- HTTP-PARSER DISPLAY --\n" << std::endl;

    std::cout << "START-LINE" << std::endl;
    std::cout << "method: " << _method << std::endl;
    std::cout << "uri: " << _uri << std::endl;
    if (!_uriQuery.empty()) {
        std::cout << "uriquery: " << std::endl;
        std::cout << std::left << std::setw(15) << "Key" << "|" << "Value" << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = _uriQuery.begin(); it != _uriQuery.end(); ++it) {
            std::cout << std::left << std::setw(15) << it->first << "|" << it->second << std::endl;
        }
    }
    std::cout << "http version: " << _httpVersion << std::endl;
    std::cout << "\nHEADERS" << std::endl;
    std::cout << std::left << std::setw(15) << "Key" << "|" << "Value" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        std::cout << std::left << std::setw(15) << it->first << "|" << it->second << std::endl;
    }
    std::cout << "\nBODY\n" << _body << std::endl;
}
