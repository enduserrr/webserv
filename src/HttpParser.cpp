/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:42:49 by eleppala          #+#    #+#             */
/*   Updated: 2025/04/04 11:02:20 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpParser.hpp"
#include "Types.hpp"

// Constructor
HttpParser::HttpParser(size_t max) : _state(0), _totalRequestSize(0), _maxBodySize(max) {}

// Destructor
HttpParser::~HttpParser() {}

bool HttpParser::startsWithMethod(std::string &input) {
    size_t firstSpace = input.find(' ');
    if (firstSpace == std::string::npos) {
        _state = 400;
        return false;
    }
    std::string method = input.substr(0, firstSpace);
    if (method == "GET" || method == "POST" || method == "DELETE") 
        return true; 
    _state = 405;
    return false; 
}

bool HttpParser::requestSize(ssize_t bytes) {
    _totalRequestSize += bytes;
    if (_totalRequestSize > (_maxBodySize + MAX_REQ_SIZE)) {
        _state = 413;
        return false;
    }
    return true;
}

// Should be moved to ServerLoop as technically isn't a part of HttpParsing
bool HttpParser::isFullRequest(std::string &input, ssize_t bytes) {
    if (!requestSize(bytes)) {
        return false;
    } 
    if (_totalRequestSize == static_cast<size_t>(bytes) && !startsWithMethod(input))
        return false;
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
            if (contentLength > _maxBodySize) {
                _state = 413; 
                return false;
            }
        } catch (const std::exception &e){
            _state = 400;
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
    return true;
}

bool HttpParser::parseRequest(ServerBlock &block) {
    std::istringstream ss(_fullRequest);
    std::string line;
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
    if (request.getMethod() == "POST" && request.getBody().size() == 0) {
        if (_state == 0)
            _state = 400;
        return false;
    }
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

// STARTLINE SYNTAX: [method] [URI] [HTTP_VERSION]
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
        _state = 400; //METHOD NOT ALLOWED
        return false;
    }
    req.setMethod(word);
    return true;
}

// Valid syntax: /dir/subdir?query=string  -- /dir/subdir?color=red&size=large
bool HttpParser::parseUri(std::istringstream &ss, HttpRequest &req) {
    std::string word;
    std::string temp;
    if (!(ss >> word)) {
        _state = 400; //BADREQ
        return false;
    }
    std::string::iterator it = word.begin();
    for (; it != word.end(); ++it) {
        if (*it == '?') {
            if (!parseUriQuery(std::string(it + 1, word.end()), req)) {
                _state = 400; // BAD REQUEST
                return false;
            }
            break;
        } else if (*it == '%' && std::distance(it, word.end()) > 2) {
            std::string hexStr = std::string(it + 1, it + 3);
            char decodedChar = static_cast<char>(std::stoi(hexStr, nullptr, 16));
            temp += decodedChar;
            it += 2;
        } else {
            temp += *it;
        }
    }
    if (!isValidUri(temp))
        return false;
    req.setUri(temp);
    return true;
}

std::string decodePercentEncoding(const std::string &str) {
    std::string decoded;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            if (std::isxdigit(str[i + 1]) && std::isxdigit(str[i + 2])) {
                std::string hexStr = str.substr(i + 1, 2);
                unsigned char decodedChar = static_cast<unsigned char>(std::stoi(hexStr, nullptr, 16));
                decoded += decodedChar;
                i += 2;
                continue;
            }
        }
        decoded += str[i];
    }
    std::cout << "decode: " << decoded << std::endl;
    return decoded;
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
            std::string key = decodePercentEncoding(token.substr(0, pos));
            std::string value = decodePercentEncoding(token.substr(pos + 1));
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
    size_t pos = line.find(':');
    if (pos == std::string::npos) {
        _state = 400;
        return false;
    }
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    key.erase(key.find_last_not_of(" \t\r\n") + 1);
    value.erase(0, value.find_first_not_of(" \t\r\n"));
    req.addNewHeader(key, value);
    return true;
}

void HttpParser::parseBody(std::string &body, HttpRequest &req) {
    Types types; // Use the centralized Types class
    std::string contentType = req.getHeader("Content-Type");
    std::cout << RB << contentType << RES << std::endl;
    std::string emptyBody = "";

    if (contentType.empty()) {
        Logger::getInstance().logLevel("INFO", "Empty body.", 0);
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
                Types type; 
                if (!type.isValidMime(filename)) {
                    _state = 415; 
                    return ;
                }
                req.setFileName(filename); // Store filename
            }
        }// ↓↓↓ Extract file content ↓↓↓
        size_t contentStart = body.find("\r\n\r\n", dispositionPos);
        if (contentStart != std::string::npos) {
            contentStart += 4;
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
    // bool matched = false;

    req.setAutoIndex(block.getAutoIndex(req.getUri()));
    try {
        req.setRoot(block.getLocation(req.getUri()).getRoot());
    } catch (const std::exception &e) {
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
