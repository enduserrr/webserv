/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:19:04 by asalo             #+#    #+#             */
/*   Updated: 2025/02/08 10:37:56 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Mime.hpp"

/* Init MIME types map */
Mime::Mime() {
    _types[".html"] = "text/html";
    _types[".htm"]  = "text/html";
    _types[".css"]  = "text/css";
    _types[".ico"]  = "image/x-icon";
    _types[".avi"]  = "video/x-msvideo";
    _types[".bmp"]  = "image/bmp";
    _types[".doc"]  = "application/msword";
    _types[".gif"]  = "image/gif";
    _types[".gz"]   = "application/x-gzip";
    _types[".jpg"]  = "image/jpeg";
    _types[".jpeg"] = "image/jpeg";
    _types[".png"]  = "image/png";
    _types[".txt"]  = "text/plain";
    _types[".mp3"]  = "audio/mp3";
    _types[".pdf"]  = "application/pdf";
    _types["default"] = "text/html";
}

Mime::~Mime() {}

std::string Mime::getMimeType(const std::string &extension) const {
    // Explicitly declare the iterator type instead of using 'auto'
    std::map<std::string, std::string>::const_iterator it = _types.find(extension);
    if (it != _types.end()) {
        return it->second;
    }
    std::map<std::string, std::string>::const_iterator defIt = _types.find("default");
    if (defIt != _types.end()) {
        return defIt->second;
    }
    throw std::runtime_error("No MIME type found and no default defined.");
}



