/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 09:23:00 by eleppala          #+#    #+#             */
/*   Updated: 2025/04/10 21:32:21 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Global.hpp"

class HttpRequest {
private:
    size_t                              _maxBodySize;
    bool                                _autoIndex;
    std::string                         _method;
    std::string                         _uri;
    std::string                         _httpVersion;
    std::string                         _body;
    std::string                         _fileName;
    std::string                         _root;
    std::map <std::string, std::string> _uriQuery;
    std::map <std::string, std::string> _headers;

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
    bool                                        getAutoIndex();
    std::string                                 getUri() const;
    std::string                                 getHttpVersion() const;
    std::string                                 getHeader(const std::string& key) const;
    std::string                                 getRoot() const;
    const std::string&                          getBody() const;
    const std::string&                          getMethod() const;
    const std::string&                          getFileName() const;
    const std::map <std::string, std::string>&  getUriQuery() const;
    const std::map <std::string, std::string>&  getHeaders() const;
    void                                        setAutoIndex(bool b);
    void                                        setRoot(const std::string &root);

    void                                        display() const;
};

#endif
