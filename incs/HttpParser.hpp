/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:45:10 by eleppala          #+#    #+#             */
/*   Updated: 2025/04/10 21:29:14 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "ServerBlock.hpp"

class HttpParser {
private:
    int                                 _state;
    size_t                              _totalRequestSize;
    size_t                              _maxBodySize;
    HttpRequest                         _request;
    std::string                         _fullRequest;
    std::string                         _pendingData;
    std::string                         _method;
    std::string                         _uri;
    std::string                         _httpVersion;
    std::string                         _body;
    std::string                         _root;
    std::string                         _redirTo;
    std::map <std::string, std::string> _headers;
    std::map <std::string, std::string> _uriQuery;


public:
    HttpParser(size_t max);
    ~HttpParser();

    void    matchRoute(ServerBlock &b, HttpRequest &req);
    bool    methodAllowed(HttpRequest &req);
    bool    startsWithMethod(std::string &input);
    bool    requestSize(ssize_t bytes);
    bool    isFullRequest(std::string &input, ssize_t bytes);
    bool    parseRequest(ServerBlock &block);
    bool    parseStartLine(std::string &line, HttpRequest &req);
    bool    parseMethod(std::istringstream &ss, HttpRequest &req);
    bool    parseUriQuery(const std::string &query, HttpRequest &req);
    bool    parseUri(std::istringstream &ss, HttpRequest &req);
    bool    isValidUri(std::string& uri);
    bool    parseVersion(std::istringstream &ss, HttpRequest &req);
    bool    createRequest(ServerBlock &block, HttpRequest &req);
    bool    parseHeader(std::string &line, HttpRequest &req);
    void    parseBody(std::string &body, HttpRequest &req);

    int                                 getState() const;
    std::string                         getRedirection() const;
    std::string                         getMethod() const;
    std::string                         getBody() const;
    std::string                         getUri() const;
    std::string                         getRoot() const;
    const HttpRequest&                  getPendingRequest() const;
    std::map<std::string, std::string>  getQueryString() const;
};

#endif
