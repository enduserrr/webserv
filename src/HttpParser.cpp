/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:42:49 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/11 12:20:48 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpParser.hpp"
#include <sstream>
#include <iomanip>

/* Constructor */
HttpParser::HttpParser() {}

/* Destructor */
HttpParser::~HttpParser() {}

bool HttpParser::readFullRequest(std::istream& input) {
    char buffer[1024];
    std::string request = _pendingData;
    _pendingData.clear();

    while (input.read(buffer, sizeof(buffer)) || input.gcount() > 0) {
        request.append(buffer, input.gcount());
        std::cout << std::endl;
        size_t headerEnd = request.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            return false;
        }
        if (headerEnd != std::string::npos) {
            size_t contentLengthPos = request.find("Content-Length:");
            if (contentLengthPos == std::string::npos) {
                _pendingData = request.substr(headerEnd + 4);
                request = request.substr(0, headerEnd + 4);
                return true;
            }

            size_t bodyStart = headerEnd + 4;
            int contentLength = std::stoi(request.substr(contentLengthPos + 15));
            if (request.size() >= bodyStart + contentLength) {
                _pendingData = request.substr(bodyStart + contentLength);
                request = request.substr(0, bodyStart + contentLength);
                return parseRequest(request, 100);
            }
        }
    }
    return false;
}

bool HttpParser::parseRequest(std::string &req, size_t max) {

    //debug
    // std::cout << req << std::endl;

    std::istringstream ss(req);
    std::string line;
    _maxBodySize = max;
    std::getline(ss, line);
    if (!parseStartLine(line)) {
        return false;
    }
    std::string headers;
    while (getline(ss, line, '\r') && ss.get() == '\n' && !line.empty()) {
        headers += line + "\n";
    }
    if (!parseHeaders(headers)) {
        return false;
    }
    std::getline(ss, line);
    if (!parseBody(line)) {
        return false;
    }
    createRequest();
    return true;
}

//STARTLINE SYNTAX: [method] [URI] [HTTP_VERSION]
bool HttpParser::parseStartLine(std::string& line) {
    std::istringstream ss(line);
    std::string word;

    ss >> word;
    if (!setMethod(word)) {
        return false;
    }
    ss >> word;
    if (!parseUri(word)) {
        return false;
    }
    ss >> word;
    if (word != "HTTP/1.1") {
        std::cout << "Wrong HTTP version"<< std::endl;
        return false;
    }
    _httpVersion = word;
    return true;
}

bool HttpParser::setMethod(std::string& method) {

    // for now just test if method get post or delete
    // later get allowed methods from config file location{allowed methods:}

    if (method != "GET" && method != "POST" && method != "DELETE") {
        std::cout << method << "No allowed method detected!"<< std::endl;
        return false;
    }
    _method = method;
    return true;
}

//valid syntax:
// /dir/subdir?query=string
// /dir/subdir?color=red&size=large
bool HttpParser::parseUri(std::string& uri){
    bool prev = false;
    for (size_t i = 0; i < uri.length(); i++) {
        if (uri[i] == '/') {
            if (prev) continue;
            prev = true;
        } else {
            prev = false;
        }
        if(uri[i] == '?') {
            if (!parseUriQuery(uri.substr(i + 1))) {
                return false;
            }
            break ;
        }
        _uri += uri[i];
    }
    if (!isValidUri(_uri)){
        std::cerr << "uri is not valid" << std::endl;
        return false;
    }
    return true;
}

bool HttpParser::parseUriQuery(std::string query) {
    if (query.empty()){
        return true;
    }
    std::stringstream ss(query);
    std::string token;
    while (std::getline(ss, token, '&')) {
        if (token.empty()) {
            std::cerr << "false query" << std::endl;
            return false;
        }
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            if (key.empty()) {
                return false;
            }
            _uriQuery[key] = value;
        } else {
            _uriQuery[token] = "true";
        }
    }
    return true;
}

bool HttpParser::isValidUri(std::string& uri) {

    //should take locations from config file and see if matches for
    //uri form reguest.

    if (uri.empty() || uri[0] != '/') {
        std::cout << "URI should start with '/'"<< std::endl;
        return false;
    }
    std::string invalidChars = "<>{}|\\^`\" ";
    for (size_t i = 0; i < uri.length(); i++) {
        if (invalidChars.find(uri[i]) != std::string::npos) {
            std::cout << "Invalid character in URI: " << uri[i] << std::endl;
            return false;
        }
    }
    return true;
}


bool HttpParser::parseHeaders(std::string& lines) {
    std::istringstream ss(lines);
    std::string line;
    while(std::getline(ss, line)){
        // std::cout << "parse: " << line << std::endl;
        size_t deli = line.find(':');
        if (deli == std::string::npos) {
            std::cerr << "headers: key + value pair not detected" << std::endl;
            return false;
        }
        std::string key = line.substr(0, deli);
        std::string value = line.substr(deli + 1);
        whiteSpaceTrim(key);
        whiteSpaceTrim(value);
        if (key.empty() || value.empty()) {
            std::cerr << "headers: key or value cannot be empty" << std::endl;
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

bool HttpParser::createRequest(){
    HttpRequest req;
    req.setBodySize(_maxBodySize);
    req.setMethod(_method);
    req.setUri(_uri);
    req.setHttpVersion(_httpVersion);
    req.setUriQuery(_uriQuery);
    req.setHeaders(_headers); //key + value
    req.setBody(_body);
    req.setAutoIndex(_autoIndex);
    req.setRoot(_root);
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
