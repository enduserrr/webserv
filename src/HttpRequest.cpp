/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 09:22:42 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/04 11:01:37 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpRequest.hpp"

/* Constructor */
HttpRequest::HttpRequest() {}

/* Destructor */
HttpRequest::~HttpRequest() {}

void HttpRequest::setBodySize(int size){
    _maxBodySize = size;
}
void HttpRequest::setMethod(std::string& method){
    _method = method;
}
void HttpRequest::setUri(std::string& uri){
    _uri = uri;
}
void HttpRequest::setHttpVersion(std::string& version){
    _httpVersion = version;
}

void HttpRequest::setUriQuery(const std::string &key, const std::string &value){
    _uriQuery[key] = value;
}
void HttpRequest::addNewHeader(std::string &key, std::string &value){
    _headers[key] = value;
}
void HttpRequest::setBody(std::string& body){
    _body = body;
}

int HttpRequest::getBodySize() {
    return _maxBodySize;
}

std::string HttpRequest::getMethod() {
    return _method;
}

std::string HttpRequest::getUri() {
    return _uri;
}

std::string HttpRequest::getHttpVersion() {
    return _httpVersion;
}

std::map <std::string, std::string> HttpRequest::getUriQuery() {
    return _uriQuery;
}

std::string& HttpRequest::getHeader(const std::string &key) {
    return _headers[key];
}

std::map <std::string, std::string> HttpRequest::getHeaders() {
    return _headers;
}

std::string HttpRequest::getBody() {
    return _body;
}
