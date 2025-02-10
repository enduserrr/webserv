/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:45:10 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/08 10:07:19 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include "Webserver.hpp"
#include "HttpRequest.hpp"


// Socket will give rawchunks of data,
// meaning there might be incomplete requests or
// one request + half of second request at same time
// or multiple requests.

class HttpParser {
private:
    std::string                         _pendingData;   //used for storing rest of the chunk
    std::vector<HttpRequest>            _requests;

    size_t                              _maxBodySize;   //get from ServerBlock
    std::string                         _method;
    std::string                         _uri;
    std::map <std::string, std::string> _uriQuery;
    std::string                         _httpVersion;
    std::map <std::string, std::string> _headers;       //key + value
    std::string                         _body;
    bool                                _autoIndex;
    std::string                         _root;

public:
    HttpParser();
    ~HttpParser();


    bool readFullRequest(std::istream& input);
    bool parseRequest(std::string& req, size_t max);
    bool parseStartLine(std::string& line);

    bool setMethod(std::string& method);
    bool parseUriQuery(std::string query);
    bool parseUri(std::string& uri);
    bool isValidUri(std::string& uri);

    bool parseHeaders(std::string& line);
    bool parseBody(std::string& line);
    void whiteSpaceTrim(std::string& str);
    void display() const;
    void removeRequest();

    /* Creates new request to HttpRequests vector after parsing */
    bool createRequest();
    std::vector<HttpRequest> &getRequests();
    HttpRequest &getPendingRequest();

    std::string getMethod() {
        return _method;
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

    bool indexAllowed() {
        return _autoIndex;
    }

    std::string getRoot() {
        return _root;
    }

    void setAutoIndex(bool b) {
        _autoIndex = b;
    }
    void setRoot(std::string root) {
    _root = root;
    }
};

#endif
