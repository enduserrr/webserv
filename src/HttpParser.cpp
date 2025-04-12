/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:42:49 by eleppala          #+#    #+#             */
/*   Updated: 2025/04/10 21:28:04 by asalo            ###   ########.fr       */
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

void HttpParser::matchRoute(ServerBlock &b, HttpRequest &req) {
    std::string uri = req.getUri(); 
    const std::map<std::string, Location>& locations = b.getLocations();
    std::string bestMatch = "";
    for (std::map<std::string, Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const std::string& locPath = it->first;
        if (uri.compare(0, locPath.length(), locPath) == 0) {
            if (locPath.length() > bestMatch.length())
                bestMatch = locPath;
        }
    }
    req.setLocation(locations.at(bestMatch)); 
}

bool HttpParser::methodAllowed(HttpRequest &req) {
    const std::vector<std::string>& allowed = req.getLocation().getAllowedMethods();
    if (std::find(allowed.begin(), allowed.end(), req.getMethod()) == allowed.end()) {
        _state = 405;
        return false;
    }
    return true;
}

bool HttpParser::parseRequest(ServerBlock &block) {
    std::istringstream ss(_fullRequest);
    std::string line;
    HttpRequest request;
    std::getline(ss, line);
    if (!parseStartLine(line, request))
        return false;
    matchRoute(block, request);
    if (!methodAllowed(request))
        return false; 
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
    if (_state != 0)
        return false; 
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
    std::string contentType = req.getHeader("Content-Type");
    // std::cout << RED << contentType << RES << std::endl;
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
            // std::cout << GC << "Extracted boundary: " << boundary << RES << std::endl;
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
                bool is_valid_mime = Types::getInstance().isValidMime(filename);
                if (is_valid_mime == false) {
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
    else if (Types::getInstance().isValidContent(contentType) == true) {
        req.setBody(body);
    }
    else {
        Logger::getInstance().logLevel("WARN", "Unsupported content-type. Setting empty body", 0);
        // std::cerr << "Error: Unsupported Content-Type: " << contentType << std::endl;
        req.setBody(emptyBody);
    }
}

bool HttpParser::createRequest(ServerBlock &block, HttpRequest &req) {
    req.setAutoIndex(block.getAutoIndex(req.getUri()));
    try {
        req.setRoot(block.getLocation(req.getUri()).getRoot());
        if (block.getLocation(req.getUri()).getRedirect().first != 0) {
            _state = block.getLocation(req.getUri()).getRedirect().first;
            _redirTo = block.getLocation(req.getUri()).getRedirect().second;
        }
    } catch (const std::exception &e) {
        req.setRoot(block.getRoot());
    }
    _request = req;
    return true;
}

const HttpRequest& HttpParser::getPendingRequest() const {
    return _request;
}

std::string HttpParser::getRedirection() const {
    return _redirTo;
}

std::string HttpParser::getMethod() const {
    return _method;
}

int HttpParser::getState() const {
    return _state;
}

std::string HttpParser::getBody() const {
    return _body;
}

std::map<std::string, std::string> HttpParser::getQueryString() const {
    return _uriQuery;
}

std::string HttpParser::getUri() const {
    return _uri;
}

std::string HttpParser::getRoot() const {
    return _root;
}