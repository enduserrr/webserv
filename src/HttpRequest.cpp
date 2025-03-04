/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 09:22:42 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/04 08:45:59 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpRequest.hpp"

/* Constructor */
HttpRequest::HttpRequest() : _maxBodySize(0) {}

/* Destructor */
HttpRequest::~HttpRequest() {}

void HttpRequest::setBodySize(size_t size) {
    _maxBodySize = size;
}
void HttpRequest::setMethod(const std::string& method){
    _method = method;
}
void HttpRequest::setUri(const std::string& uri){
    _uri = uri;
}
void HttpRequest::setHttpVersion(const std::string& version){
    _httpVersion = version;
}

void HttpRequest::setUriQuery(const std::string &key, const std::string &value) {
    _uriQuery[key] = value;
}
void HttpRequest::addNewHeader(const std::string &key, const std::string &value) {
    _headers[key] = value;
}
void HttpRequest::setBody(const std::string& body){
    _body = body;
}

size_t HttpRequest::getBodySize() const {
    return _maxBodySize;
}

const std::string& HttpRequest::getMethod() const {
    return _method;
}

std::string HttpRequest::getUri() const {
    return _uri;
}

std::string HttpRequest::getHttpVersion() const {
    return _httpVersion;
}

const std::map <std::string, std::string> &HttpRequest::getUriQuery() const {
    return _uriQuery;
}

std::string     HttpRequest::getHeader(const std::string &key) const {
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it == _headers.end())
        return "";
    return it->second;
}

const std::map <std::string, std::string>& HttpRequest::getHeaders() const {
    return _headers;
}

const std::string&  HttpRequest::getBody() const {
    return _body;
}

void HttpRequest::setFileName(const std::string& name) {
    _fileName = name;
}

const std::string& HttpRequest::getFileName() const {
    return _fileName;
}
