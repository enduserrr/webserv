/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:42:49 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/16 13:03:02 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpParser.hpp"
#include "Types.hpp"
#include <sstream>
#include <iomanip>

/* Constructor */
HttpParser::HttpParser() : _state(0) {}

/* Destructor */
HttpParser::~HttpParser() {}

bool HttpParser::startsWithMethod(const std::string &input) {
    size_t firstSpace = input.find(' ');
    if (firstSpace == std::string::npos)
        return false;  // No space means malformed request

    std::string method = input.substr(0, firstSpace);
    return (method == "GET" || method == "POST" || method == "DELETE");
}

/* Should be moved to ServerLoop as technically isn't a part of HttpParsing */
bool HttpParser::isFullRequest(std::string &input) {
    if (!startsWithMethod(input)) {
        _state = 400;
        input.clear();
        return false;
    }
    if (input.back() == '/') //Not sure if this is doing anything
        return true;
    size_t headerEnd = input.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;
    size_t bodyStart = headerEnd + 4;
    size_t contentLengthPos = input.find("Content-Length:");
    size_t contentLength = 0;

    if (contentLengthPos != std::string::npos) {
        size_t contentLengthEnd = input.find("\r\n", contentLengthPos);
        if (contentLengthEnd == std::string::npos)
            return false;
        std::string contentLengthValue = input.substr(contentLengthPos + 15, contentLengthEnd - (contentLengthPos + 15));
        try {
            contentLength = std::stoi(contentLengthValue);
        } catch (const std::exception &e){
            _state = 400;
            input.clear();
            return false;
        }
        if (input.size() < bodyStart + contentLength)
            return false;
    }
    _fullRequest = input.substr(0, bodyStart + contentLength);
    if (bodyStart + contentLength <= input.size())
        input = input.substr(bodyStart + contentLength);
    else
        input.clear();
    std::cout << GC << "\n\nLEFT OVER:" << input << RES << std::endl;
    return true;
}

bool HttpParser::parseRequest(ServerBlock &block) {
    (void)block;
    std::istringstream ss(_fullRequest);
    std::string line;
    _maxBodySize = block.getBodySize();
    HttpRequest request;
    std::getline(ss, line);
    if (!parseStartLine(line, request)) {
        return false;
    }
    while (getline(ss, line, '\r') && ss.get() == '\n' && !line.empty()) {
        if (!parseHeader(line, request))
            return false;
    }
    std::string body;
    body.assign(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>());
    parseBody(body, request);
    createRequest(block, request);
    return true;
}

bool HttpParser::parseVersion(std::istringstream &ss, HttpRequest &req) {
    std::string word;
    if (!(ss >> word)) {
        _state = 400; //BADREQ
        return false;
    }
    if (word != "HTTP/1.1") {
        _state = 505; //VERSION NOT SUPPORTED
        return false;
    }
    req.setHttpVersion(word);
    return true;
}

//STARTLINE SYNTAX: [method] [URI] [HTTP_VERSION]
bool HttpParser::parseStartLine(std::string &line, HttpRequest &req) {
    std::istringstream ss(line);
    if (!parseMethod(ss, req))
        return false;
    if (!parseUri(ss, req))
        return false;
    if (!parseVersion(ss, req))
        return false;
    return true;
}

bool HttpParser::parseMethod(std::istringstream &ss, HttpRequest &req) {
    std::string word;
    if (!(ss >> word)) {
        _state = 400; //BADREQ
        return false;
    }
    if (word != "GET" && word != "POST" && word != "DELETE") {
        _state = 405; //METHOD NOT ALLOWED
        return false;
    }
    req.setMethod(word);
    return true;
}

//valid syntax: /dir/subdir?query=string  -- /dir/subdir?color=red&size=large
bool HttpParser::parseUri(std::istringstream &ss, HttpRequest &req) {
    std::string word;
    std::string temp;
    if (!(ss >> word)) {
        _state = 400; //BADREQ
        return false;
    }
    for (size_t i = 0; i < word.length(); i++) {
        if(word[i] == '?') {
            if (!parseUriQuery(word.substr(i + 1), req)) {
                _state = 400; //BADREQ
                return false;
            } else {
                break ;
            }

        }
        temp += word[i];
    }
    if (!isValidUri(temp))
        return false;
    req.setUri(temp);
    return true;
}

bool HttpParser::parseUriQuery(const std::string &query, HttpRequest &req) {
    if (query.empty())
        return false;
    std::stringstream ss(query);
    std::string token;
    while (std::getline(ss, token, '&')) {
        if (token.empty())
            return false;
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            if (key.empty())
                return false;
            req.setUriQuery(key, value);
        } else {
            req.setUriQuery(token, "true");
        }
    }
    return true;
}

bool HttpParser::isValidUri(std::string& uri) {
    if (uri.empty() || uri[0] != '/') {
        _state = 400; //BADREQ
        return false;
    }
    std::string invalidChars = "<>{}|\\^`\" ";
    for (size_t i = 0; i < uri.length(); i++) {
        if (invalidChars.find(uri[i]) != std::string::npos) {
            _state = 400; //BADREQ
            return false;
        }
    }
    return true;
}

bool HttpParser::parseHeader(std::string &line, HttpRequest &req) {
    std::istringstream ss(line);
    std::string key;
    std::string value;
    if (!(ss >> key >> value)){
        _state = 400; //BADREQ
        return false;
    }
    key.pop_back();
    if (key == "Content-Length") {
        size_t size = std::stoi(value);
        if (size > _maxBodySize) {
            _state = 413; //PAYLOAD TOO LARGE
            return false;
        }
    }
    req.addNewHeader(key, value);
    return true;
}

void HttpParser::parseBody(std::string &body, HttpRequest &req) {
    Types types; // Use the centralized Types class
    std::string contentType = req.getHeader("Content-Type");
    std::cout << RB << contentType << RES << std::endl;
    std::string emptyBody = "";

    if (contentType.empty()) {
        std::cerr << RED << "Empty Body" << RES << std::endl;
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
            std::cout << GC << "Extracted boundary: " << boundary << RES << std::endl;
        } else {
            std::cerr << RED << "Error: Missing boundary in Content-Type: " << RES << contentType << std::endl;
        }
        std::string boundary = "--" + contentType.substr(boundaryPos + 9);
        size_t dispositionPos = body.find("Content-Disposition: form-data;");
        if (dispositionPos == std::string::npos) {
            std::cerr << RED << "Error: Missing Content-Disposition header" << RES << std::endl;
            req.setBody(emptyBody);
            return;
        }// ↓↓↓ EXTRACT FILENAME ↓↓↓
        size_t filenamePos = body.find("filename=\"", dispositionPos);
        if (filenamePos != std::string::npos) {
            filenamePos += 10; // Move past "filename=\""
            size_t endPos = body.find("\"", filenamePos);
            if (endPos != std::string::npos) {
                std::string filename = body.substr(filenamePos, endPos - filenamePos);
                req.setFileName(filename); // Store filename
            }
        }// ↓↓↓ Extract file content ↓↓↓
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
        if (req.getUri().find(locations[i].getPath()) == 0) {
            req.setAutoIndex(locations[i].getPath(), locations[i].getAutoIndex());
            req.setRoot(locations[i].getRoot());
            matched = true;
            break;
        }
    }
    if (!matched) {// If no matching location, use the ServerBlock's global settings.
        req.setAutoIndex(block.getRoot(), block.getAutoIndex());
        req.setRoot(block.getRoot());
    }
    _requests.push_back(req);
    return true;
}

std::vector<HttpRequest>& HttpParser::getRequests() {
    return _requests;
}

HttpRequest& HttpParser::getPendingRequest() {
    return _requests.front();
}

void HttpParser::removeRequest() {
    if (!_requests.empty()) {
        _requests.erase(_requests.begin());
    }
}

void HttpParser::display() const {

    std::cout << GC << "\n\n-- HTTP-PARSER DISPLAY --\n" << std::endl;

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
    std::cout << "\nBODY\n" << _body << RES << std::endl;
}
