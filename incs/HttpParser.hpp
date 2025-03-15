/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:45:10 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/15 15:33:42 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include "Webserver.hpp"
#include "HttpRequest.hpp"
#include "ServerBlock.hpp"


// Socket will give rawchunks of data,
// meaning there might be incomplete requests or
// one request + half of second request at same time
// or multiple requests.

class HttpParser {
private:
    int                                 _state;

    std::string                         _fullRequest;
    std::string                         _pendingData;   //used for storing rest of the chunk
    std::vector<HttpRequest>            _requests;

    size_t                              _maxBodySize;   //get from ServerBlock
    std::string                         _method;
    std::string                         _uri;
    std::map <std::string, std::string> _uriQuery;
    std::string                         _httpVersion;
    std::map <std::string, std::string> _headers;       //key + value
    std::string                         _body;
    // bool                                _autoIndex;
    std::string                         _root;

public:
    HttpParser();
    ~HttpParser();


    bool startsWithMethod(const std::string &input);
    bool isFullRequest(std::string &input);
    // bool readFullRequest(std::istream &input, ServerBlock &block);
    bool parseRequest(ServerBlock &block);
    bool parseStartLine(std::string &line, HttpRequest &req);

    bool parseMethod(std::istringstream &ss, HttpRequest &req);
    bool parseUriQuery(const std::string &query, HttpRequest &req);
    bool parseUri(std::istringstream &ss, HttpRequest &req);
    bool isValidUri(std::string& uri);
    bool parseVersion(std::istringstream &ss, HttpRequest &req);

    bool createRequest(ServerBlock &block, HttpRequest &req);

    bool parseHeader(std::string &line, HttpRequest &req);
    void parseBody(std::string &body, HttpRequest &req);
    void whiteSpaceTrim(std::string& str);
    void display() const;
    void removeRequest();

    /* Creates new request to HttpRequests vector after parsing */
    // bool createRequest();
    std::vector<HttpRequest> &getRequests();
    HttpRequest &getPendingRequest();

    std::string getMethod() {
        return _method;
    }

    int getState() {
        return _state;
    }

    std::string getBody() {
        return _body;
    }

    std::map<std::string, std::string> getQueryString() {
        return _uriQuery;
    }

    std::string getUri() {
        return _uri;
    }

    // bool indexAllowed() {
    //     return _autoIndex;
    // }

    std::string getRoot() {
        return _root;
    }

    // void setAutoIndex(bool b) {
    //     _autoIndex = b;
    // }
    void setRoot(std::string root) {
    _root = root;
    }
};

#endif
