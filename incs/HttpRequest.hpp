/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 09:23:00 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/16 12:43:21 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <map>
#include <vector>

class HttpRequest {
private:
    size_t                              _maxBodySize;
    std::string                         _method;
    std::string                         _uri;
    std::string                         _httpVersion;

    std::map <std::string, std::string> _uriQuery;
    std::map <std::string, std::string> _headers;
    std::string                         _body;
    std::string                         _fileName;

    // std::map<std::string, bool>         _indexLoc;
    bool                                _autoIndex;
    std::string                         _root;

public:
    HttpRequest();
    ~HttpRequest();

    void                                        setBodySize(size_t value);
    void                                        setMethod(const std::string& method);
    void                                        setUri(const std::string& uri);
    void                                        setHttpVersion(const std::string& version);
    void                                        setUriQuery(const std::string& key, const std::string& value);
    void                                        addNewHeader(const std::string& key, const std::string& value);
    void                                        setBody(const std::string& headers);
    void                                        setFileName(const std::string& name);

    size_t                                      getBodySize() const;
    const std::string&                          getMethod() const;
    std::string                                 getUri() const;
    std::string                                 getHttpVersion() const;
    std::string                                 getHeader(const std::string& key) const;
    const std::string&                          getBody() const;
    const std::string&                          getFileName() const;
    const std::map <std::string, std::string>&  getUriQuery() const;
    const std::map <std::string, std::string>&  getHeaders() const;
    bool                                        getAutoIndex() {
        return _autoIndex;
    }

    /**
     * @brief   Returns true if auto index in a given location (key) is set to true
     */
    // bool    getIndexLoc(std::string key) {
    //     std::cout << "Location key: " << key << std::endl;
    //     if (_indexLoc[key] == true)
    //         return true;
    //     return false;
    // }

    std::string getRoot() {
        return _root;
    }

    /**
     * @brief   Set auto index on/off (b) for a given key (location)
     */
    void setAutoIndex(bool b) {
        _autoIndex = b;
    }

    void setRoot(const std::string &root) {
        _root = root;
    }

    void display() const {
    std::cout << "---- HttpRequest Display ----" << std::endl;
    std::cout << "Method: " << _method << std::endl;
    std::cout << "URI: " << _uri << std::endl;
    std::cout << "HTTP Version: " << _httpVersion << std::endl;
    std::cout << "Max Body Size: " << _maxBodySize << std::endl;
    std::cout << "Root: " << _root << std::endl;
    std::cout << "File Name: " << _fileName << std::endl;
    std::cout << "Body: " << _body << std::endl;

    std::cout << "Headers:" << std::endl;
    for (const auto &header : _headers) {
        std::cout << "  " << header.first << ": " << header.second << std::endl;
    }

    std::cout << "URI Query:" << std::endl;
    for (const auto &query : _uriQuery) {
        std::cout << "  " << query.first << ": " << query.second << std::endl;
    }

    std::cout << "-----------------------------" << std::endl;
    }
};

#endif
