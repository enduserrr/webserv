/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 09:23:00 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/07 18:32:09 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <map>

class HttpRequest {
private:
    size_t                              _maxBodySize;
    std::string                         _method;
    std::string                         _uri;
    std::string                         _httpVersion;

    std::map <std::string, std::string> _uriQuery;
    std::map <std::string, std::string> _headers;       //key + value
    std::string                         _body;

    bool                                _autoIndex;
    std::string                         _root;

public:
    HttpRequest();
    ~HttpRequest();

    void                                setBodySize(int value);
    void                                setMethod(std::string& method);
    void                                setUri(std::string& uri);
    void                                setHttpVersion(std::string& version);
    void                                setUriQuery(const std::string &key, const std::string &value);
    void                                addNewHeader(std::string &key, std::string &value);
    void                                setBody(std::string& headers);

    int                                 getBodySize();
    std::string                         getMethod();
    std::string                         getUri();
    std::string                         getHttpVersion();
    std::map <std::string, std::string> getUriQuery();
    std::map <std::string, std::string> getHeaders();
    std::string&                        getHeader(const std::string &key);
    std::string                         getBody();

    bool getAutoIndex() {
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
